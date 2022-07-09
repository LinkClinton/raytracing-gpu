#include "model_generator.hpp"

path_tracing::runtime::resources::mesh_cpu_buffer path_tracing::extensions::models::generate_sphere_model(
	real radius, uint32 slice, uint32 stack)
{
	mesh_cpu_buffer buffer;

	buffer.positions.push_back(vector3(0.0f, +radius, 0.0f));
	buffer.uvs.push_back(vector3(0.0f, 0.0f, 0.0f));
	buffer.normals.push_back(vector3(0.0f, +1.0f, 0.0f));

	const auto phi_step = pi<real>() / stack;
	const auto theta_step = two_pi<real>() / slice;

	for (size_t index0 = 1; index0 < stack; index0++) {
		const auto phi = index0 * phi_step;

		for (size_t index1 = 0; index1 <= slice; index1++) {
			const auto theta = index1 * theta_step;

			buffer.positions.push_back(vector3(
				radius * sin(phi) * cos(theta),
				radius * cos(phi),
				radius * sin(phi) * sin(theta)
			));

			buffer.uvs.push_back(vector3(
				theta / two_pi<real>() * 4,
				phi / pi<real>() * 2,
				0.0f
			));

			buffer.normals.push_back(normalize(buffer.positions.back()));
		}
	}

	buffer.positions.push_back(vector3(0.0f, -radius, 0.0f));
	buffer.uvs.push_back(vector3(0.0f, 1.0f, 0.0f));
	buffer.normals.push_back(vector3(0.0f, -1.0f, 0.0f));

	for (size_t index = 1; index <= slice; index++) {
		buffer.indices.push_back(0);
		buffer.indices.push_back(static_cast<uint32>(index + 1));
		buffer.indices.push_back(static_cast<uint32>(index));
	}

	size_t base_index = 1;
	size_t ring_vertex_count = slice + 1;

	for (size_t index0 = 0; index0 < stack - 2; index0++) {
		for (size_t index1 = 0; index1 < slice; index1++) {
			buffer.indices.push_back(static_cast<uint32>(base_index + index0 * ring_vertex_count + index1));
			buffer.indices.push_back(static_cast<uint32>(base_index + index0 * ring_vertex_count + index1 + 1));
			buffer.indices.push_back(static_cast<uint32>(base_index + (index0 + 1) * ring_vertex_count + index1));

			buffer.indices.push_back(static_cast<uint32>(base_index + (index0 + 1) * ring_vertex_count + index1));
			buffer.indices.push_back(static_cast<uint32>(base_index + index0 * ring_vertex_count + index1 + 1));
			buffer.indices.push_back(static_cast<uint32>(base_index + (index0 + 1) * ring_vertex_count + index1 + 1));
		}
	}

	size_t south_pole_index = buffer.positions.size();

	base_index = south_pole_index - ring_vertex_count;

	for (size_t index = 0; index < slice; index++) {
		buffer.indices.push_back(static_cast<uint32>(south_pole_index));
		buffer.indices.push_back(static_cast<uint32>(base_index + index));
		buffer.indices.push_back(static_cast<uint32>(base_index + index + 1));
	}

	return buffer;
}
