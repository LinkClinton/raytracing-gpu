#include "camera.hpp"

path_tracing::core::cameras::camera_gpu_buffer::camera_gpu_buffer(
	const matrix4x4& raster_to_camera,
	const matrix4x4& camera_to_world) :
	raster_to_camera(raster_to_camera), camera_to_world(camera_to_world)
{
}

path_tracing::core::cameras::camera::camera(
	const shared::transform& projective, 
	const shared::transform& transform,
	real focus, real lens) :
	mCameraToScreen(projective), mCameraToWorld(transform),
	mFocus(focus), mLens(lens)
{
}

void path_tracing::core::cameras::camera::set_transform(const shared::transform& transform)
{
	mCameraToWorld = transform;
}

path_tracing::core::cameras::camera_gpu_buffer path_tracing::core::cameras::camera::gpu_buffer(size_t width, size_t height) const noexcept
{
	const auto screen_window_max = vector3(1);
	const auto screen_window_min = vector3(-1);
	
	auto screen_to_raster = scale(
		vector3(
			static_cast<float>(width),
			static_cast<float>(height),
			static_cast<float>(1)
		));

	screen_to_raster = screen_to_raster * scale(
		vector3(
			1.0f / (screen_window_max.x - screen_window_min.x),
			1.0f / (screen_window_min.y - screen_window_max.y),
			1.0f));

	screen_to_raster = screen_to_raster * translate(
		glm::vec3(
			-screen_window_min.x,
			-screen_window_max.y,
			0
		));

	const auto raster_to_screen = inverse(screen_to_raster);
	const auto raster_to_camera = inverse(mCameraToScreen) * raster_to_screen;

	return camera_gpu_buffer(raster_to_camera.matrix, mCameraToWorld.matrix);
}

path_tracing::core::shared::transform path_tracing::core::cameras::camera::camera_to_world() const noexcept
{
	return mCameraToWorld;
}
