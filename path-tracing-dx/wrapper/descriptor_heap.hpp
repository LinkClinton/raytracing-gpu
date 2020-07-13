#pragma once

#include "device.hpp"

namespace path_tracing::dx::wrapper {

	class descriptor_table final : public noncopyable {
	public:
		descriptor_table() = default;

		~descriptor_table() = default;

		void add_srv_range(const std::vector<std::string>& name, size_t base, size_t space);

		void add_uav_range(const std::vector<std::string>& name, size_t base, size_t space);

		void add_cbv_range(const std::vector<std::string>& name, size_t base, size_t space);

		void add_range(const std::vector<std::string>& name, const D3D12_DESCRIPTOR_RANGE_TYPE& type, size_t base, size_t space);

		D3D12_ROOT_PARAMETER parameter() const;
		
		size_t index(const std::string& name) const;
		
		size_t count() const noexcept;
	private:
		std::unordered_map<std::string, size_t> mDescriptorsIndex;

		std::vector<D3D12_DESCRIPTOR_RANGE> mRanges;
	};
	
	class descriptor_heap final : public noncopyable {
	public:
		explicit descriptor_heap(const std::shared_ptr<device>& device, const D3D12_DESCRIPTOR_HEAP_TYPE& type, size_t count);

		explicit descriptor_heap(const std::shared_ptr<device>& device, const std::shared_ptr<descriptor_table>& table);
		
		~descriptor_heap() = default;

		ID3D12DescriptorHeap* const* get_address_of() const;
		ID3D12DescriptorHeap* operator->() const;
		ID3D12DescriptorHeap* get() const;

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(size_t index = 0) const;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(size_t index = 0) const;

		size_t count() const noexcept;
	private:
		ComPtr<ID3D12DescriptorHeap> mHeap = nullptr;

		size_t mOffset = 0;
		size_t mCount = 0;
	};

}
