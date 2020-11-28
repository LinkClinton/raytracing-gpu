#include "entity.hpp"

path_tracing::real path_tracing::scenes::compute_mesh_area_with_transform(const runtime::resources::meshes_system& system, 
	const transform& transform, const mesh_info& info)
{
	const auto& positions = system.cpu_buffer().positions;
	const auto& indices = system.cpu_buffer().indices;

	real area = 0;
	
	for (uint32 index = 0; index < info.idx_count; index += 3) {
		std::array<vector3, 3> points = {
			positions[info.vtx_location + indices[info.idx_location + index + 0]],
			positions[info.vtx_location + indices[info.idx_location + index + 1]],
			positions[info.vtx_location + indices[info.idx_location + index + 2]]
		};

		points[0] = transform_point(transform, points[0]);
		points[1] = transform_point(transform, points[1]);
		points[2] = transform_point(transform, points[2]);

		area += static_cast<real>(0.5) * length(cross(points[2] - points[0], points[2] - points[1]));
	}

	return area;
}
