#pragma once

#include "../../runtime/runtime_service.hpp"
#include "../../scenes/scene.hpp"

#include "json.hpp"

namespace raytracing::extensions::json
{

	using runtime::runtime_service;
	
	class json_scene_loader final : public noncopyable
	{
	public:
		static void load(const runtime_service& service, const nlohmann::json& scene, const std::string& directory);

		static void load(const runtime_service& service, const std::string& filename);
	};
	
}
