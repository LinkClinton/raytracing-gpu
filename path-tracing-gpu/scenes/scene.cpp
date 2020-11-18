#include "scene.hpp"

std::tuple<path_tracing::matrix4x4, path_tracing::matrix4x4> path_tracing::scenes::compute_camera_matrix(
	const scene& scene)
{
	const auto screen_window_max = vector3(1);
	const auto screen_window_min = vector3(-1);

	const auto camera_to_screen = perspective_fov(scene.camera_system, glm::radians(scene.camera.fov),
		scene.camera.resolution.x, scene.camera.resolution.y,
		0.1f, 1000.f);
	
	auto screen_to_raster = scale(matrix4x4(1),
		vector3(
			static_cast<float>(scene.camera.resolution.x),
			static_cast<float>(scene.camera.resolution.y),
			static_cast<float>(1)
		));

	screen_to_raster = screen_to_raster * scale(matrix4x4(1),
		vector3(
			1.0f / (screen_window_max.x - screen_window_min.x),
			1.0f / (screen_window_min.y - screen_window_max.y),
			1.0f));

	screen_to_raster = screen_to_raster * translate(matrix4x4(1),
		glm::vec3(
			-screen_window_min.x,
			-screen_window_max.y,
			0
		));

	const auto raster_to_screen = inverse(screen_to_raster);
	const auto raster_to_camera = glm::inverse(camera_to_screen) * raster_to_screen;

	return {
		glm::transpose(raster_to_camera),
		glm::transpose(scene.camera.transform.matrix())
	};
}
