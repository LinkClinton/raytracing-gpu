#pragma once

#include "device.hpp"

namespace path_tracing::dx::wrapper {

	class command_allocator final : public noncopyable {
	public:
		explicit command_allocator(const std::shared_ptr<device>& device);

		~command_allocator() = default;

		ID3D12CommandAllocator* const* get_address_of() const;
		ID3D12CommandAllocator* operator->() const;
		ID3D12CommandAllocator* get() const;
	private:
		ComPtr<ID3D12CommandAllocator> mAllocator = nullptr;
	};

	class graphics_command_list final : public noncopyable {
	public:
		explicit graphics_command_list(
			const std::shared_ptr<command_allocator>& allocator,
			const std::shared_ptr<device>& device);

		~graphics_command_list() = default;

		ID3D12GraphicsCommandList4* const* get_address_of() const;
		ID3D12GraphicsCommandList4* operator->() const;
		ID3D12GraphicsCommandList4* get() const;
	public:
		ComPtr<ID3D12GraphicsCommandList4> mCommandList = nullptr;
	};

	class command_queue final : public noncopyable {
	public:
		explicit command_queue(const std::shared_ptr<device>& device);

		~command_queue() = default;

		ID3D12CommandQueue* const* get_address_of() const;
		ID3D12CommandQueue* operator->() const;
		ID3D12CommandQueue* get() const;

		void execute(const std::vector<std::shared_ptr<graphics_command_list>>& command_lists) const;

		void wait();
	private:
		ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;
		ComPtr<ID3D12Fence> mFence = nullptr;

		size_t mFenceValue = 0;
	};
	
}
