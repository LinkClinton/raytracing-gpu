#pragma once

#include "../interfaces/indexable.hpp"
#include "image.hpp"

#include <memory>

namespace path_tracing::core::textures {

	using namespace shared;

	struct texture_gpu_buffer {
		constexpr static inline uint32 null = std::numeric_limits<uint32>::max();

		vector3 scale = vector3(1);
		
		uint32 index = null;
		
		texture_gpu_buffer() = default;
	};

	class texture final : public interfaces::noncopyable, public interfaces::indexable {
	public:
		explicit texture(const std::shared_ptr<image>& image, const vector3& scale);

		~texture() = default;

		texture_gpu_buffer gpu_buffer() const noexcept;

		std::shared_ptr<image> image() const noexcept;

		static uint32 count() noexcept;
	private:
		static inline uint32 mIndexCount = 0;
		
		std::shared_ptr<textures::image> mImage;
		
		vector3 mScale;
	};

}
