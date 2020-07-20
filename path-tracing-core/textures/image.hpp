#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../interfaces/indexable.hpp"
#include "../shared/transform.hpp"

#include <vector>

namespace path_tracing::core::textures {

	using namespace shared;
	
	class image final : public interfaces::noncopyable, public interfaces::indexable {
	public:
		explicit image(const std::vector<vector3>& values, size_t width, size_t height);

		explicit image(const std::vector<vector2>& values, size_t width, size_t height);
		
		explicit image(const std::vector<real>& values, size_t width, size_t height);

		~image() = default;

		const std::vector<vector3>& vector3() const noexcept;

		const std::vector<vector2>& vector2() const noexcept;
		
		const std::vector<real>& real() const noexcept;

		size_t width() const noexcept;

		size_t height() const noexcept;

		static uint32 count() noexcept;
	private:
		static inline uint32 mIndexCount = 0;
		
		std::vector<shared::vector3> mVector3Values;
		std::vector<shared::vector2> mVector2Values;
		std::vector<core::real> mRealValues;

		size_t mWidth;
		size_t mHeight;
	};
	
}
