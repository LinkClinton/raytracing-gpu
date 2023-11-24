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
	std::vector<resources::gpu_mesh> upload_gpu_mesh_heaps;

	for (const auto& entity : service.scene.entities)
	{
		if (entity.mesh.has_value())
		{
			// vertex attribute gpu buffer
			if (!service.resource_system.has<resources::gpu_mesh>(entity.mesh->name))
			{
				const auto& [info, data] = service.resource_system.resource<resources::cpu_mesh>(entity.mesh->name);

				// empty resource(maybe normals or uvs in general)
				// will create with other buffer size but copy zero data
				resources::gpu_mesh resource =
				{
					.info = info,
					.data = resources::gpu_mesh_data
					{
						.positions = wrapper::directx12::buffer::create(
							service.render_device.device(),
							wrapper::directx12::resource_info::common(),
							info.vtx_count * sizeof(vector3)),
						.normals = wrapper::directx12::buffer::create(
							service.render_device.device(),
							wrapper::directx12::resource_info::common(),
							info.vtx_count * sizeof(vector3)),
						.uvs = wrapper::directx12::buffer::create(
							service.render_device.device(),
							wrapper::directx12::resource_info::common(),
							info.vtx_count * sizeof(vector3)),
						.indices = wrapper::directx12::buffer::create(
							service.render_device.device(),
							wrapper::directx12::resource_info::common(),
							info.idx_count * sizeof(uint32))
					}
				};
				
				resources::gpu_mesh upload =
				{
					.info = info,
					.data = resources::gpu_mesh_data
					{
						.positions = wrapper::directx12::buffer::create(
							service.render_device.device(),
							wrapper::directx12::resource_info::upload(),
							info.vtx_count * sizeof(vector3)),
						.normals = wrapper::directx12::buffer::create(
							service.render_device.device(),
							wrapper::directx12::resource_info::upload(),
							info.vtx_count * sizeof(vector3)),
						.uvs = wrapper::directx12::buffer::create(
							service.render_device.device(),
							wrapper::directx12::resource_info::upload(),
							info.vtx_count * sizeof(vector3)),
						.indices =  wrapper::directx12::buffer::create(
							service.render_device.device(),
							wrapper::directx12::resource_info::upload(),
							info.idx_count * sizeof(uint32))
					}
				};

				upload.data.positions.copy_from_cpu(data.positions.data(), data.positions.size() * sizeof(vector3));
				upload.data.normals.copy_from_cpu(data.normals.data(), data.normals.size() * sizeof(vector3));
				upload.data.uvs.copy_from_cpu(data.uvs.data(), data.uvs.size() * sizeof(vector3));
				upload.data.indices.copy_from_cpu(data.indices.data(), data.indices.size() * sizeof(uint32));

				resource.data.positions.copy_from(command_list, upload.data.positions);
				resource.data.normals.copy_from(command_list, upload.data.normals);
				resource.data.uvs.copy_from(command_list, upload.data.uvs);
				resource.data.indices.copy_from(command_list, upload.data.indices);

				command_list.resource_barrier(
					{
					resource.data.positions.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
					resource.data.normals.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
					resource.data.uvs.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
					resource.data.indices.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
				});

				upload_gpu_mesh_heaps.emplace_back(upload);

				service.resource_system.add(entity.mesh->name, std::move(resource));
			}

			// ray tracing bottom level accelerate structure
			if (!service.resource_system.has<wrapper::directx12::raytracing_geometry>(entity.mesh->name))
			{
				const auto& [info, data] = service.resource_system.resource<resources::gpu_mesh>(entity.mesh->name);

				wrapper::directx12::raytracing_geometry geometry = wrapper::directx12::raytracing_geometry::create(
					D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE,
					command_list, service.render_device.device(),
					data.positions->GetGPUVirtualAddress(), info.vtx_count,
					data.indices->GetGPUVirtualAddress(), info.idx_count
				);

				service.resource_system.add(entity.mesh->name, std::move(geometry));
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
