#include "tiny_obj_loader.hpp"

path_tracing::runtime::resources::mesh_cpu_buffer path_tracing::extensions::models::load_obj_mesh(
	const std::string& filename)
{
	std::vector<tinyobj::material_t> materials;
	std::vector<tinyobj::shape_t> shapes;
	tinyobj::attrib_t attribute;

	std::string warning;
	std::string error;

	const auto ret = LoadObj(&attribute, &shapes, &materials, &warning, &error, filename.c_str());

	if (!ret) return {};
	
	for (const auto& shape : shapes) {
		std::vector<unsigned> indices;
		std::vector<vector3> positions;
		std::vector<vector3> normals;
		std::vector<vector3> uvs;

		for (const auto& index : shape.mesh.indices) {

			if (attribute.vertices.empty() == false)
				positions.push_back(vector3(
					attribute.vertices[3ULL * index.vertex_index + 0],
					attribute.vertices[3ULL * index.vertex_index + 1],
					attribute.vertices[3ULL * index.vertex_index + 2]
				));

			if (attribute.normals.empty() == false)
				normals.push_back(vector3(
					attribute.normals[3ULL * index.normal_index + 0],
					attribute.normals[3ULL * index.normal_index + 1],
					attribute.normals[3ULL * index.normal_index + 2]
				));

			if (attribute.texcoords.empty() == false)
				uvs.push_back(vector3(
					attribute.texcoords[2ULL * index.texcoord_index + 0],
					attribute.texcoords[2ULL * index.texcoord_index + 1],
					0
				));

			indices.push_back(static_cast<unsigned>(indices.size()));
		}

		return { positions, normals, uvs, indices };
	}

	return {};
}
