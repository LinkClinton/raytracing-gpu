#pragma once

#include "interfaces/noncopyable.hpp"
#include "renderer_backend.hpp"

#include <string>

namespace path_tracing::core {

	class application_base : public interfaces::noncopyable {
	public:
		explicit application_base(const std::string& name, int width, int height);

		~application_base();

		void initialize();

		void load(
			const std::shared_ptr<camera>& camera,
			const std::shared_ptr<scene>& scene);
		
		void run_loop();
	protected:
		virtual void initialize_windows();

		virtual void initialize_graphics() = 0;
	protected:
		std::shared_ptr<renderer_backend> mRenderer = nullptr;

		std::shared_ptr<camera> mCamera;
		std::shared_ptr<scene> mScene;
		
		std::string mName;

		int mWidth, mHeight;

		bool mExisted;
		
		void* mHandle;
	};
	
}
