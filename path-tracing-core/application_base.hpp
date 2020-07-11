#pragma once

#include "interfaces/noncopyable.hpp"

#include <string>

namespace path_tracing::core {

	class application_base : public interfaces::noncopyable {
	public:
		explicit application_base(const std::string& name, int width, int height);

		~application_base();

		void initialize();
		
		void run_loop();
	protected:
		virtual void initialize_windows();

		virtual void initialize_graphics() = 0;
	protected:
		std::string mName;

		int mWidth, mHeight;

		bool mExisted;
		
		void* mHandle;
	};
	
}
