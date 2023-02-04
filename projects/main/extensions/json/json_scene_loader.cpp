#include "json_scene_loader.hpp"

#include <filesystem>
#include <fstream>

namespace raytracing::extensions::json {

}

void raytracing::extensions::json::json_scene_loader::load(const runtime_service& service, 
	const nlohmann::json& scene, const std::string& directory)
{
	if (scene.contains("config") == true) {
		const auto config = scene["config"];

		if (config.contains("coordinate_system")) {
			service.scene.camera_system = config["coordinate_system"]["camera"] == "right_hand" ? 
				coordinate_system::right_hand : coordinate_system::left_hand;
			service.scene.texture_system = config["coordinate_system"]["texture"] == "right_hand" ?
				coordinate_system::right_hand : coordinate_system::left_hand;
		}

		if (config.contains("output_window")) {
			const auto output_window = config["output_window"];
			
			service.scene.output_window = scenes::output_window_property{
				output_window["name"], output_window["font"],
				output_window["size"][0], output_window["size"][1],
				output_window["enable"]
			};
		}

		if (config.contains("output_images")) {
			service.scene.output_images = scenes::output_images_property{
				config["output_images"]["sdr"],
				config["output_images"]["hdr"]
			};
		}
		
		if (config.contains("sample_count"))
			service.scene.sample_count = config["sample_count"];

		if (config.contains("max_depth"))
			service.scene.max_depth = config["max_depth"];
	}

	if (scene.contains("film")) {
		service.scene.film = scenes::components::film{
			scene["film"]["resolution"][0], scene["film"]["resolution"][1],
			scene["film"]["scale"]
		};
	}

	service.render_device.wait();
}

void raytracing::extensions::json::json_scene_loader::load(const runtime_service& service, const std::string& filename)
{
	std::ifstream stream(filename);

	nlohmann::json scene; stream >> scene;

	load(service, scene, std::filesystem::path(filename).parent_path().generic_string() + "/");

	stream.close();
}
