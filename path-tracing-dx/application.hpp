#pragma once

#include "../path-tracing-core/application_base.hpp"

namespace path_tracing::dx {

	class application final : public core::application_base {
	public:
		explicit application(const std::string& name, int width, int height);

		~application();
	protected:
		void initialize_graphics() override;
	};
	
}
