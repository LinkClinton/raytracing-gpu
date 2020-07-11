#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../shared/transform.hpp"

namespace path_tracing::core::shapes {

	class shape : public interfaces::noncopyable {
	public:
		explicit shape(bool reverse_orientation);

		~shape() = default;

		bool reverse_orientation() const noexcept;
	private:
		bool mReverseOrientation = false;
	};	
}
