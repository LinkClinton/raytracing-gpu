#include "json_scene_loader.hpp"

#include <filesystem>
#include <fstream>

using namespace path_tracing::runtime::resources;
using namespace path_tracing::scenes::components;

namespace path_tracing::extensions::json {

	transform load_transform_from_matrix_property(const nlohmann::json& json)
	{
		matrix4x4 matrix;

		matrix[0][0] = json[0];  matrix[0][1] = json[1];  matrix[0][2] = json[2];  matrix[0][3] = json[3];
		matrix[1][0] = json[4];  matrix[1][1] = json[5];  matrix[1][2] = json[6];  matrix[1][3] = json[7];
		matrix[2][0] = json[8];  matrix[2][1] = json[9];  matrix[2][2] = json[10]; matrix[2][3] = json[11];
		matrix[3][0] = json[12]; matrix[3][1] = json[13]; matrix[3][2] = json[14]; matrix[3][3] = json[15];

		return transform(matrix);
	}

	transform load_transform_from_look_at_property(const coordinate_system& system, const nlohmann::json& json)
	{
		const vector3 eye = json["eye"];
		const vector3 at = json["at"];
		const vector3 up = json["up"];

		return transform(look_at(system, eye, at, up)).inverse();
	}

	transform load_transform_from_property(const coordinate_system& system, const nlohmann::json& json)
	{
		if (json.contains("matrix")) return load_transform_from_matrix_property(json["matrix"]);
		if (json.contains("eye")) return load_transform_from_look_at_property(system, json);

		throw "not implementation";
	}

	perspective_camera load_perspective_camera(const coordinate_system& system, const nlohmann::json& json)
	{
		return perspective_camera{
			load_transform_from_property(system, json["transform"]),
			json["resolution"].get<vector2>(),
			json["fov"].get<real>()
		};
	}

	mesh_info load_mesh_from_data_property(const meshes_system& system, const nlohmann::json& json)
	{
		mesh_cpu_buffer instance;

		instance.positions = json["positions"].get<std::vector<vector3>>();
		instance.indices = json["indices"].get<std::vector<uint32>>();

		if (json.contains("normals")) instance.normals = json["normals"].get<std::vector<vector3>>();
		
	}
}

void path_tracing::extensions::json::json_scene_loader::load(const runtime_service& service, 
	const nlohmann::json& scene, const std::string& directory)
{
	
}

void path_tracing::extensions::json::json_scene_loader::load(const runtime_service& service, const std::string& filename)
{
	std::ifstream stream(filename);

	nlohmann::json scene; stream >> scene;

	load(service, scene, std::filesystem::path(filename).parent_path().generic_string() + "/");

	stream.close();
}