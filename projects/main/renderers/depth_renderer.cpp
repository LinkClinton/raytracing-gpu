#include "depth_renderer.hpp"

raytracing::renderers::depth_renderer::depth_renderer(const runtime_service& service) : renderer(service)
{
	mCommandAllocator = wrapper::directx12::command_allocator::create(service.render_device.device());
	mCommandList = wrapper::directx12::graphics_command_list::create(service.render_device.device(), mCommandAllocator);

	mCommandAllocator.reset();
	mCommandList.reset(mCommandAllocator);

	// create top acceleration 
	{
		std::vector<wrapper::directx12::raytracing_instance> instances;

		for (const auto& entity : service.scene.entities)
		{
			if (entity.mesh.has_value())
			{
				wrapper::directx12::raytracing_instance instance;
				
				instance.geometry = service.resource_system.resource<wrapper::directx12::raytracing_geometry>(entity.mesh->name);

				const auto matrix = transpose(entity.transform.matrix());

				for (int r = 0; r < 3; r++)
				{
					for (int c = 0; c < 4; c++)
					{
						instance.transform[r][c] = matrix[r][c];
					}
				}
			}
		}

		mAcceleration = wrapper::directx12::raytracing_acceleration::create(
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE,
			mCommandList, service.render_device.device(),
			instances);
	}

}

void raytracing::renderers::depth_renderer::update(const runtime_service& service, const runtime_frame& frame)
{
}

void raytracing::renderers::depth_renderer::render(const runtime_service& service, const runtime_frame& frame)
{
}
