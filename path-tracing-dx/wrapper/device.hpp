#pragma once

#include "utility.hpp"

namespace path_tracing::dx::wrapper {

	class device final : public noncopyable {
	public:
		explicit device(const D3D_FEATURE_LEVEL& level);

		~device() = default;

		ID3D12Device5* const* get_address_off() const;
		ID3D12Device5* operator->() const;
		ID3D12Device5* get() const;
	private:
		ComPtr<ID3D12Device5> mDevice = nullptr;
	};
	
}
