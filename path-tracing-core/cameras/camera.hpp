#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../shared/transform.hpp"

namespace path_tracing::core::cameras {

	using namespace shared;

	struct camera_gpu_buffer {
		transform projective;
		transform transform;

		real focus = 0, lens = 0;

		real unused0 = 0, unused1 = 0;

		camera_gpu_buffer() = default;

		camera_gpu_buffer(
			const shared::transform& projective,
			const shared::transform& transform,
			real focus, real lens);
	};
	
	class camera final : public interfaces::noncopyable {
	public:
		explicit camera(
			const shared::transform& projective,
			const shared::transform& transform,
			real focus, real lens);

		~camera() = default;

		transform projective() const noexcept;

		transform transform() const noexcept;

		camera_gpu_buffer gpu_buffer() const noexcept;
	private:
		shared::transform mProjective;
		shared::transform mTransform;

		real mFocus, mLens;
	};
	
}
