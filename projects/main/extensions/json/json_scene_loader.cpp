#include "json_scene_loader.hpp"

#include "../textures/texture_loader.hpp"
#include "../models/model_loader.hpp"

#include <filesystem>
#include <fstream>
#include <format>

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
		uint32 scene_internal_mesh_count = 0;

		for (const auto& entity : entities)
		{
			scenes::entity entity_data;
			
			if (entity.contains("material"))
			{
				scenes::submodule_data material;

				for (const auto& property : entity["material"].items())
				{
					if (property.key() == "type")
					{
						material.type = property.value();
					}else
					{
						material.textures[property.key()] = property.value();
						
						if (!material.textures[property.key()].image.empty())
						{
							if (!context.service.resource_system.has<runtime::resources::cpu_texture>(material.textures[property.key()].image))
							{
								runtime::resources::cpu_texture resource = textures::load_texture(
									context.working_directory + material.textures[property.key()].image);

								context.service.resource_system.add(material.textures[property.key()].image, std::move(resource));
							}
						}
					}
				}

				entity_data.material = material;
			}
			
			if (entity.contains("light"))
			{
				scenes::submodule_data light;

				for (const auto& property : entity["light"].items())
				{
					if (property.key() == "type")
					{
						light.type = property.value();
					}
					else
					{
						light.textures[property.key()] = property.value();

						if (!light.textures[property.key()].image.empty())
						{
							if (!context.service.resource_system.has<runtime::resources::cpu_texture>(light.textures[property.key()].image))
							{
								runtime::resources::cpu_texture resource = textures::load_texture(
									context.working_directory + light.textures[property.key()].image);

								context.service.resource_system.add(light.textures[property.key()].image, std::move(resource));
							}
						}
					}
				}

				entity_data.light = light;
			}

			if (entity.contains("shape"))
			{
				scenes::submodule_mesh mesh;

				if (entity["shape"]["type"] == "triangles")
				{
					runtime::resources::cpu_mesh resource;

					resource.data.positions = entity["shape"]["triangles"]["positions"];
					resource.data.indices = entity["shape"]["triangles"]["indices"].get<std::vector<uint32>>();

					if (entity["shape"]["triangles"].contains("normals"))
					{
						resource.info.attribute |= runtime::resources::mesh_attribute::normal;
						resource.data.normals = entity["shape"]["triangles"]["normals"];
					}

					if (entity["shape"]["triangles"].contains("uvs"))
					{
						resource.info.attribute |= runtime::resources::mesh_attribute::uv;
						resource.data.uvs = entity["shape"]["triangles"]["uvs"];
					}

					resource.info.vtx_count = static_cast<uint32>(resource.data.positions.size());
					resource.info.idx_count = static_cast<uint32>(resource.data.indices.size());
					resource.info.attribute = resource.info.attribute | runtime::resources::mesh_attribute::position;
					resource.info.attribute = resource.info.attribute | runtime::resources::mesh_attribute::index;

					std::string mesh_name = std::format("scene_internal_mesh_{}", scene_internal_mesh_count++);
					
					context.service.resource_system.add(mesh_name, std::move(resource));

					mesh.name = mesh_name;
				}

				if (entity["shape"]["type"] == "mesh")
				{
					mesh.name = entity["shape"]["mesh"];

					if (!context.service.resource_system.has<runtime::resources::cpu_mesh>(mesh.name))
					{
						auto resource = models::load_mesh(context.working_directory + mesh.name);

						context.service.resource_system.add(mesh.name, std::move(resource));
					}
				}
				
				entity_data.mesh = mesh;
			}

			entity_data.transform = scenes::transform(entity["transform"]);

			context.service.scene.entities.push_back(std::move(entity_data));
		}
	}
}

void raytracing::extensions::json::json_scene_loader::load(const runtime_service& service, 
	const nlohmann::json& scene, const std::string& directory)
{
	const scene_loader_context context =
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

	if (scene.contains("entities"))
	{
		load_scene_entities(context, scene["entities"]);
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
