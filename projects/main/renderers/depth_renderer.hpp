#pragma once

#include "renderer.hpp"

namespace raytracing::renderers
{
	class depth_renderer final : public renderer
	{
	public:
		explicit depth_renderer(const runtime_service& service);

		void update(const runtime_service& service, const runtime_frame& frame) override;

		void render(const runtime_service& service, const runtime_frame& frame) override;
	private:
		wrapper::directx12::command_allocator mCommandAllocator;
		wrapper::directx12::graphics_command_list mCommandList;
	};
}
