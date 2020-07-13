#pragma once

#include "utility.hpp"

namespace path_tracing::dx::wrapper {

	class device final {
	public:
		device() = default;

		~device() = default;

		ID3D12Device5* const* get_address_of() const;
		ID3D12Device5* operator->() const;
		ID3D12Device5* get() const;

		static device create();
	private:
		ComPtr<ID3D12Device5> mDevice = nullptr;
	};
	
}
