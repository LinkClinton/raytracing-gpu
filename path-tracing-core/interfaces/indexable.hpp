#pragma once

namespace path_tracing::core {

	using uint32 = unsigned;
	
}

namespace path_tracing::core::interfaces {

	class indexable {
	public:
		indexable(uint32 index) : mIndex(index) {};

		virtual ~indexable() = default;
		
		uint32 index() const noexcept { return mIndex; }
	protected:
		uint32 mIndex = 0;
	};
	
}
