#include "json_scene_loader.hpp"

#include <filesystem>
#include <fstream>

namespace raytracing::extensions::json
{
	struct scene_loader_context
	{
		runtime_service service;

		std::string working_directory;
	};
	
	void load_scene_config(const scene_loader_context& context, const nlohmann::json& config)
	{
		if (config.contains("output_window"))
		{
			const auto output_window = config["output_window"];

			context.service.scene.output_window = scenes::output_window_property
			{
				output_window["name"], output_window["font"],
				output_window["size"][0], output_window["size"][1],
				output_window["enable"]
			};
		}

		if (config.contains("output_images"))
		{
			context.service.scene.output_images = scenes::output_images_property
			{
				config["output_images"]["sdr"],
				config["output_images"]["hdr"]
			};
		}

		if (config.contains("sample_count"))
		{
			context.service.scene.sample_count = config["sample_count"];
		}

		if (config.contains("max_depth"))
		{
			context.service.scene.max_depth = config["max_depth"];
		}
	}

	void load_scene_film(const scene_loader_context& context, const nlohmann::json& film)
	{
		context.service.scene.film =
		{
			film["resolution"][0],
			film["resolution"][1],
			film["scale"]
		};
	}

	void load_scene_camera(const scene_loader_context& context, const nlohmann::json& camera)
	{
		context.service.scene.camera =
		{
			scenes::transform(camera["transform"]),
			camera["perspective"]["fov"]
		};
	}

	void load_scene_entities(const scene_loader_context& context, const nlohmann::json& entities)
	{
		for (const auto& entity : entities)
		{

		}
	}
}

void raytracing::extensions::json::json_scene_loader::load(const runtime_service& service, 
	const nlohmann::json& scene, const std::string& directory)
{
	scene_loader_context context =
	{
		service,
		directory
	};

	if (scene.contains("camera")) 
	{
		load_scene_camera(context, scene["camera"]);
	}

	if (scene.contains("config")) 
	{
		load_scene_config(context, scene["config"]);
	}
	
	if (scene.contains("film")) 
	{
		load_scene_film(context, scene["film"]);
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
