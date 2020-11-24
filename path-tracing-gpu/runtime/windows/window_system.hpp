#pragma once

#include "../../extensions/imgui/imgui_renderer.hpp"

#include "view_window.hpp"

namespace path_tracing::runtime {

	struct runtime_service;
	struct runtime_frame;
	
}

namespace path_tracing::runtime::windows {

	using extensions::imgui::imgui_renderer;
	
	class window_system final : public noncopyable {
	public:
		window_system() = default;

		~window_system() = default;

		void resolve(const runtime_service& service);

		void release(const runtime_service& service);
		
		void update(const runtime_service& service, const runtime_frame& frame);

		void render(const runtime_service& service, const runtime_frame& frame);
		
		bool living() const noexcept;

		bool active() const noexcept;
	private:
		imgui_renderer mImGuiRenderer;

		view_window mViewWindow;

		ImTextureID mRenderTargetID = nullptr;
	};
	
}
