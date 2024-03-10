#include "render_system.hpp"

#include "../runtime_service.hpp"
#include "../runtime_frame.hpp"

#include "../../renderers/depth_renderer.hpp"
#include "../../renderers/shade_renderer.hpp"
#include "../../renderers/renderer.hpp"

#include <ranges>

namespace raytracing::runtime::render
{

	template <typename T>
	void copy_memory(
		const wrapper::directx12::graphics_command_list& command_list,
		const wrapper::directx12::texture2d& destination, 
		const wrapper::directx12::buffer& upload, 
		const std::vector<T>& values)
	{
		const auto logic_width = destination.size_x() *
			wrapper::directx12::size_of(destination.format());

		if (destination.alignment() == logic_width)
		{
			upload.copy_from_cpu(values.data(), sizeof(T) * values.size());
		}
		else 
		{
			const auto upload_memory = static_cast<byte*>(upload.begin_mapping());
			const auto cpu_memory = reinterpret_cast<const byte*>(values.data());
			const auto alignment = destination.alignment();

			for (size_t y = 0; y < destination.size_y(); y++) 
			{
				std::memcpy(
					upload_memory + y * alignment, 
					cpu_memory + y * logic_width,
					logic_width);
			}

			upload.end_mapping();
		}

		destination.copy_from(command_list, upload);
		destination.barrier(command_list, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

}

void raytracing::runtime::render::render_system::resolve(const runtime_service& service)
{
	auto command_allocator = wrapper::directx12::command_allocator::create(service.render_device.device());
	auto command_list = wrapper::directx12::graphics_command_list::create(service.render_device.device(), command_allocator);

	command_allocator.reset();
	command_list.reset(command_allocator);

	std::vector<wrapper::directx12::buffer> upload_gpu_texture_heaps;

	uint32 geometry_buffer_vtx_count = 0;
	uint32 geometry_buffer_idx_count = 0;

	for (const auto& entity : service.scene.entities)
	{
		if (entity.mesh.has_value())
		{
			// vertex attribute gpu buffer
			if (!service.resource_system.has<resources::geometry>(entity.mesh->name))
			{
				const auto& [info, data] = service.resource_system.resource<resources::mesh>(entity.mesh->name);

				// will create geometry data after we create geometry buffer
				resources::geometry resource =
				{
					.info = resources::geometry_info
					{
						.vtx_location = geometry_buffer_vtx_count,
						.idx_location = geometry_buffer_idx_count,
						.vtx_count = info.vtx_count,
						.idx_count = info.idx_count
					},
					.data = resources::geometry_data
					{
					}
				};

				// update the vertex and index buffer count
				geometry_buffer_vtx_count += info.vtx_count;
				geometry_buffer_idx_count += info.idx_count;

				service.resource_system.add(entity.mesh->name, std::move(resource));
			}
		}

		// merge material and light textures
		std::vector<mapping<std::string, scenes::texture>> textures = 
		{
			entity.material.has_value() ? entity.material->textures : mapping<std::string, scenes::texture>{},
			entity.light.has_value() ? entity.light->textures : mapping<std::string, scenes::texture>{}
		};

		// load gpu texture from cpu texture
		for (const auto& texture :  textures | std::views::join | std::views::values)
		{
			if (!texture.image.empty() && !service.resource_system.has<resources::gpu_texture>(texture.image))
			{
				const auto& [info, data] = service.resource_system.resource<resources::cpu_texture>(texture.image);

				resources::gpu_texture resource =
				{
					.info = info,
					.data = wrapper::directx12::texture2d::create(
						service.render_device.device(),
						wrapper::directx12::resource_info::common(),
						resources::to_dxgi_format(info.format, info.channel),
						info.size_x,
						info.size_y)
				};

				wrapper::directx12::buffer upload = wrapper::directx12::buffer::create(
					service.render_device.device(),
					wrapper::directx12::resource_info::upload(),
					info.size_y * resource.data.alignment());

				copy_memory(command_list, resource.data, upload, data);

				upload_gpu_texture_heaps.emplace_back(upload);

				service.resource_system.add(texture.image, std::move(resource));
			}
		}
	}

	const size_t geometry_buffer_vtx_size = geometry_buffer_vtx_count * sizeof(vector3);
	const size_t geometry_buffer_idx_size = geometry_buffer_idx_count * sizeof(uint32);

	resources::geometry_buffer geometry_upload =
	{
		.info = resources::geometry_buffer_info
		{
			.vtx_count = geometry_buffer_vtx_count,
			.idx_count = geometry_buffer_idx_count
		},
		.data = resources::geometry_buffer_data
		{
			.positions = wrapper::directx12::buffer::create(
				service.render_device.device(),
				wrapper::directx12::resource_info::upload(),
				geometry_buffer_vtx_size),
			.normals = wrapper::directx12::buffer::create(
				service.render_device.device(),
				wrapper::directx12::resource_info::upload(),
				geometry_buffer_vtx_size),
			.uvs = wrapper::directx12::buffer::create(
				service.render_device.device(),
				wrapper::directx12::resource_info::upload(),
				geometry_buffer_vtx_size),
			.indices = wrapper::directx12::buffer::create(
				service.render_device.device(),
				wrapper::directx12::resource_info::upload(),
				geometry_buffer_idx_size)
		}
	};

	// create mega geometry buffer store all mesh data
	{
		auto positions = std::vector<vector3>(geometry_buffer_vtx_count);
		auto normals = std::vector<vector3>(geometry_buffer_vtx_count);
		auto uvs = std::vector<vector3>(geometry_buffer_vtx_count);
		auto indices = std::vector<uint32>(geometry_buffer_idx_count);

		for (const auto& entity : service.scene.entities)
		{
			if (entity.mesh.has_value())
			{
				const resources::geometry& geometry = service.resource_system.resource<resources::geometry>(entity.mesh->name);
				const resources::mesh& mesh = service.resource_system.resource<resources::mesh>(entity.mesh->name);
				
				std::ranges::copy(mesh.data.positions, positions.begin() + geometry.info.vtx_location);
				std::ranges::copy(mesh.data.normals, normals.begin() + geometry.info.vtx_location);
				std::ranges::copy(mesh.data.uvs, uvs.begin() + geometry.info.vtx_location);
				std::ranges::copy(mesh.data.indices, indices.begin() + geometry.info.idx_location);
			}
		}
		
		geometry_upload.data.positions.copy_from_cpu(positions.data(), positions.size() * sizeof(vector3));
		geometry_upload.data.normals.copy_from_cpu(normals.data(), normals.size() * sizeof(vector3));
		geometry_upload.data.uvs.copy_from_cpu(uvs.data(), uvs.size() * sizeof(vector3));
		geometry_upload.data.indices.copy_from_cpu(indices.data(), indices.size() * sizeof(uint32));

		resources::geometry_buffer geometry_resource =
		{
			.info = resources::geometry_buffer_info
			{
				.vtx_count = geometry_buffer_vtx_count,
				.idx_count = geometry_buffer_idx_count
			},
			.data = resources::geometry_buffer_data
			{
				.positions = wrapper::directx12::buffer::create(
					service.render_device.device(),
					wrapper::directx12::resource_info::common(),
					geometry_buffer_vtx_size),
				.normals = wrapper::directx12::buffer::create(
					service.render_device.device(),
					wrapper::directx12::resource_info::common(),
					geometry_buffer_vtx_size),
				.uvs = wrapper::directx12::buffer::create(
					service.render_device.device(),
					wrapper::directx12::resource_info::common(),
					geometry_buffer_vtx_size),
				.indices = wrapper::directx12::buffer::create(
					service.render_device.device(),
					wrapper::directx12::resource_info::common(),
					geometry_buffer_idx_size),
			}
		};

		geometry_resource.data.positions.copy_from(command_list, geometry_upload.data.positions);
		geometry_resource.data.normals.copy_from(command_list, geometry_upload.data.normals);
		geometry_resource.data.uvs.copy_from(command_list, geometry_upload.data.uvs);
		geometry_resource.data.indices.copy_from(command_list, geometry_upload.data.indices);

		command_list.resource_barrier({
			geometry_resource.data.positions.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			geometry_resource.data.normals.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			geometry_resource.data.uvs.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			geometry_resource.data.indices.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		});

		service.resource_system.add("GeometryBuffer", std::move(geometry_resource));
	}

	// create geometry
	{
		const resources::geometry_buffer& buffer = service.resource_system.resource<resources::geometry_buffer>("GeometryBuffer");
		
		for (const auto& entity : service.scene.entities)
		{
			if (entity.mesh.has_value())
			{
				auto& [info, data] = service.resource_system.resource<resources::geometry>(entity.mesh->name);

				const auto vtx_address_offset = info.vtx_location * sizeof(vector3);
				const auto idx_address_offset = info.idx_location * sizeof(uint32);

				data.geometry = wrapper::directx12::raytracing_geometry::create(
					D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE,
					command_list, service.render_device.device(),
					buffer.data.positions->GetGPUVirtualAddress() + vtx_address_offset, info.vtx_count,
					buffer.data.indices->GetGPUVirtualAddress() + idx_address_offset,  info.idx_count
				);
			}
		}
	}

	command_list.close();

	service.render_device.queue().execute({ command_list });
	service.render_device.wait();

	mRenderTargetHDR.info = resources::texture_info
	{
		service.scene.film.size_x,
		service.scene.film.size_y,
		4,
		resources::texture_format::fp32
	};

	mRenderTargetHDR.data = wrapper::directx12::texture2d::create
	(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		service.scene.film.size_x,
		service.scene.film.size_y
	);

	mRenderTargetSDR.info = resources::texture_info
	{
		service.scene.film.size_x,
		service.scene.film.size_y,
		4,
		resources::texture_format::fp8
	};

	mRenderTargetSDR.data = wrapper::directx12::texture2d::create
	(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		service.scene.film.size_x,
		service.scene.film.size_y
	);
	
	service.resource_system.add<resources::gpu_texture>("RenderSystem.RenderTarget.HDR", mRenderTargetHDR);
	service.resource_system.add<resources::gpu_texture>("RenderSystem.RenderTarget.SDR", mRenderTargetSDR);

	//todo : create renderer from config
	mRenderer = std::make_shared<renderers::shade_renderer>(service);
}

void raytracing::runtime::render::render_system::release(const runtime_service& service)
{
}

void raytracing::runtime::render::render_system::update(const runtime_service& service, const runtime_frame& frame)
{
	if (mRenderer == nullptr) return;

	mRenderer->update(service, frame);
}

void raytracing::runtime::render::render_system::render(const runtime_service& service, const runtime_frame& frame)
{
	if (mRenderer == nullptr) return;

	mRenderer->render(service, frame);
}

raytracing::uint32 raytracing::runtime::render::render_system::sample_index() const noexcept
{
	return mRenderer ? mRenderer->sample_index() : 1;
}
