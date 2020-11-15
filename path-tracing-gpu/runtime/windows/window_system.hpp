#pragma once

#include "../../extensions/imgui/imgui_renderer.hpp"

#include "view_window.hpp"

namespace path_tracing::runtime {

	struct runtime_service;
	
}

namespace path_tracing::runtime::windows {

	using extensions::imgui::imgui_renderer;
	
	class window_system final : public noncopyable {
	public:
		window_system() = default;

		~window_system() = default;

		void resolve(const runtime_service& service);

		void update(const runtime_service& service, real delta);

		void render(const runtime_service& service, real delta);
		
		bool living() const noexcept;
	private:
		imgui_renderer mImGuiRenderer;
		
		view_window mViewWindow;

		ImTextureID mRenderTargetID = nullptr;
	};
	
}
