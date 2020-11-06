#pragma once

#include "../../runtime/runtime_service.hpp"
#include "../../scenes/scene.hpp"

#include "json.hpp"

namespace path_tracing::extensions::json {

	using namespace runtime;
	using namespace scenes;

	class json_scene_loader final : public noncopyable {
	public:
		json_scene_loader() = default;

		~json_scene_loader() = default;

		void load(const runtime_service& service, const nlohmann::json& scene, const std::string& directory = "./");

		void load(const runtime_service& service, const std::string& filename);
	};
	
}
