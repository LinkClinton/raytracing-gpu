#include "sphere.hpp"

path_tracing::core::shapes::sphere::sphere(real radius, bool reverse_orientation) :
	shape(reverse_orientation), mRadius(radius)
{
}

path_tracing::core::shapes::mesh_data path_tracing::core::shapes::sphere::mesh() const
{
	mesh_data data;

	const size_t slice_count = 64;
	const size_t stack_count = 64;

	data.positions.push_back(vector3(0, +mRadius, 0));
	data.normals.push_back(vector3(0, 1, 0));
	data.uvs.push_back(vector3(0, 0, 0));
	
	const auto phi_step = glm::pi<float>() / stack_count;
	const auto theta_step = glm::two_pi<float>() / slice_count;

	for (size_t index0 = 1; index0 < stack_count; index0++) {
		const auto phi = index0 * phi_step;

		for (size_t index1 = 0; index1 <= slice_count; index1++) {
			const auto theta = index1 * theta_step;

			vector3 position;
			vector3 uv;
			
			position.x = mRadius * glm::sin(phi) * glm::cos(theta);
			position.y = mRadius * glm::cos(phi);
			position.z = mRadius * glm::sin(phi) * glm::sin(theta);
			
			uv.x = theta / glm::two_pi<float>() * 4;
			uv.y = phi / glm::pi<float>() * 2;
			uv.z = 0;

			data.positions.push_back(position);
			data.normals.push_back(normalize(position));
			data.uvs.push_back(uv);
		}
	}

	data.positions.push_back(vector3(0, -mRadius, 0));
	data.normals.push_back(vector3(0, -1, 0));
	data.uvs.push_back(vector3(0, 0, 0));

	for (size_t index = 1; index <= slice_count; index++) {
		data.indices.push_back(0);
		data.indices.push_back(static_cast<unsigned>(index + 1));
		data.indices.push_back(static_cast<unsigned>(index));
	}

	size_t base_index = 1;
	size_t ring_vertex_count = slice_count + 1;

	for (size_t index0 = 0; index0 < stack_count - 2; index0++) {
		for (size_t index1 = 0; index1 < slice_count; index1++) {
			data.indices.push_back(static_cast<unsigned>(base_index + index0 * ring_vertex_count + index1));
			data.indices.push_back(static_cast<unsigned>(base_index + index0 * ring_vertex_count + index1 + 1));
			data.indices.push_back(static_cast<unsigned>(base_index + (index0 + 1) * ring_vertex_count + index1));

			data.indices.push_back(static_cast<unsigned>(base_index + (index0 + 1) * ring_vertex_count + index1));
			data.indices.push_back(static_cast<unsigned>(base_index + index0 * ring_vertex_count + index1 + 1));
			data.indices.push_back(static_cast<unsigned>(base_index + (index0 + 1) * ring_vertex_count + index1 + 1));
		}
	}

	const size_t south_pole_index = data.positions.size() - 1;

	base_index = south_pole_index - ring_vertex_count;

	for (size_t index = 0; index < slice_count; index++) {
		data.indices.push_back(static_cast<unsigned>(south_pole_index));
		data.indices.push_back(static_cast<unsigned>(base_index + index));
		data.indices.push_back(static_cast<unsigned>(base_index + index + 1));
	}

	return data;
}

path_tracing::core::shapes::shape_gpu_buffer path_tracing::core::shapes::sphere::gpu_buffer() const noexcept
{
	const auto mesh_data = mesh();

	return shape_gpu_buffer(
		static_cast<uint32>(mesh_data.positions.size()), 
		static_cast<uint32>(mesh_data.normals.size()),
		static_cast<uint32>(mesh_data.indices.size()),
		static_cast<uint32>(mesh_data.uvs.size()));
}

path_tracing::core::real path_tracing::core::shapes::sphere::radius() const noexcept
{
	return mRadius;
}
