#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../shared/transform.hpp"

namespace path_tracing::core::cameras {

	using namespace shared;

	struct camera_gpu_buffer {
		matrix4x4 raster_to_camera;
		matrix4x4 camera_to_world;

		real focus;
		real lens;
		
		camera_gpu_buffer() = default;
	};
	
	class camera final : public interfaces::noncopyable {
	public:
		explicit camera(
			const shared::transform& projective,
			const shared::transform& transform,
			real focus, real lens);

		~camera() = default;

		void set_transform(const shared::transform& transform);
		
		camera_gpu_buffer gpu_buffer(size_t width, size_t height) const noexcept;
		
		transform camera_to_world() const noexcept;
	private:
		transform mCameraToScreen;
		transform mCameraToWorld;

		real mFocus, mLens;
	};
	
}
