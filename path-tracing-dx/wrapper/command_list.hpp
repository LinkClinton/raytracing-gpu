#pragma once

#include "device.hpp"

namespace path_tracing::dx::wrapper {

	class command_allocator final {
	public:
		command_allocator() = default;

		~command_allocator() = default;

		ID3D12CommandAllocator* const* get_address_of() const;
		ID3D12CommandAllocator* operator->() const;
		ID3D12CommandAllocator* get() const;

		static command_allocator create(const device& device);
	private:
		ComPtr<ID3D12CommandAllocator> mAllocator = nullptr;
	};

	class graphics_command_list final {
	public:
		graphics_command_list() = default;

		~graphics_command_list() = default;

		ID3D12GraphicsCommandList4* const* get_address_of() const;
		ID3D12GraphicsCommandList4* operator->() const;
		ID3D12GraphicsCommandList4* get() const;
		
		static graphics_command_list create(const device& device, const command_allocator& allocator);
	public:
		ComPtr<ID3D12GraphicsCommandList4> mCommandList = nullptr;
	};

	class command_queue final {
	public:
		command_queue() = default;

		~command_queue() = default;

		ID3D12CommandQueue* const* get_address_of() const;
		ID3D12CommandQueue* operator->() const;
		ID3D12CommandQueue* get() const;

		void execute(const std::vector<graphics_command_list>& command_lists) const;

		void wait();

		static command_queue create(const device& device);
	private:
		ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;
		ComPtr<ID3D12Fence> mFence = nullptr;

		size_t mFenceValue = 0;
	};
	
}
