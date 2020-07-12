#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

#include <memory>
#include <vector>

namespace path_tracing::dx {

	using Microsoft::WRL::ComPtr;

	size_t size_of(const DXGI_FORMAT& format);

	size_t align_to(size_t value, size_t alignment);

	D3D12_CPU_DESCRIPTOR_HANDLE offset_handle(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, size_t value);
	
	class device final {
	public:
		device() = default;

		~device() = default;

		ID3D12Device4* operator->() const noexcept { return mDevice.Get(); }
		ID3D12Device4* get() const noexcept { return mDevice.Get(); }

		static device create();
	private:
		ComPtr<ID3D12Device4> mDevice = nullptr;
	};

	class command_allocator final {
	public:
		command_allocator() = default;

		~command_allocator() = default;

		ID3D12CommandAllocator* operator->() const noexcept { return mAllocator.Get(); }
		ID3D12CommandAllocator* get() const noexcept { return mAllocator.Get(); }

		static command_allocator create(const device& device);
	private:
		ComPtr<ID3D12CommandAllocator> mAllocator = nullptr;
	};
	
	class graphics_command_list final {
	public:
		graphics_command_list() = default;

		~graphics_command_list() = default;

		ID3D12GraphicsCommandList4* operator->() const noexcept { return mCommandList.Get(); }
		ID3D12GraphicsCommandList4* get() const noexcept { return mCommandList.Get(); }

		static graphics_command_list create(const device& device, const command_allocator& allocator);
	public:
		ComPtr<ID3D12GraphicsCommandList4> mCommandList = nullptr;
	};

	class command_queue final {
	public:
		command_queue() = default;

		~command_queue() = default;

		ID3D12CommandQueue* operator->() const noexcept { return mCommandQueue.Get(); }
		ID3D12CommandQueue* get() const noexcept { return mCommandQueue.Get(); }

		void wait();
		
		static command_queue create(const device& device);
	private:
		ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;
		ComPtr<ID3D12Fence> mFence = nullptr;

		size_t mFenceValue = 0;
	};

	class swap_chain final {
	public:
		swap_chain() = default;

		~swap_chain() = default;

		IDXGISwapChain4* operator->() const noexcept { return mSwapChain.Get(); }
		IDXGISwapChain4* get() const noexcept { return mSwapChain.Get(); }

		DXGI_FORMAT format() const noexcept;

		D3D12_CPU_DESCRIPTOR_HANDLE render_target_view(size_t index) const;
		
		void resize(const device& device, int width, int height) const;

		void present() const;
		
		static swap_chain create(const device& device, const command_queue& queue, 
			int width, int height, void* handle);
	private:
		ComPtr<ID3D12DescriptorHeap> mRTVHeap = nullptr;
		ComPtr<IDXGISwapChain4> mSwapChain = nullptr;

		DXGI_FORMAT mFormat = DXGI_FORMAT_UNKNOWN;

		size_t mDescriptorOffset = 0;
		size_t mBufferCount = 2;
	};
	
	class resource {
	public:
		resource() = default;

		~resource() = default;

		ID3D12Resource* operator->() const noexcept { return mResource.Get(); }
		ID3D12Resource* get() const noexcept { return mResource.Get(); }
		
		static D3D12_RESOURCE_BARRIER barrier_transition(
			const D3D12_RESOURCE_STATES& before,
			const D3D12_RESOURCE_STATES& after,
			const resource& resource);
	protected:
		ComPtr<ID3D12Resource> mResource = nullptr;
	};

	class buffer final : public resource {
	public:
		buffer() = default;

		~buffer() = default;

		size_t size() const noexcept;

		void* map() const;

		void unmap() const;

		static buffer create(
			const D3D12_RESOURCE_STATES& state,
			const D3D12_HEAP_PROPERTIES& heap,
			const D3D12_RESOURCE_DESC& desc,
			const device& device);
		
		static D3D12_RESOURCE_DESC resource_desc(size_t size, const D3D12_RESOURCE_FLAGS& flags);

		static buffer upload_type(
			const D3D12_RESOURCE_STATES& state,
			const D3D12_RESOURCE_FLAGS& flags,
			const device& device, size_t size);

		static buffer default_type(
			const D3D12_RESOURCE_STATES & state,
			const D3D12_RESOURCE_FLAGS & flags,
			const device& device, size_t size);
	private:
		size_t mSize = 0;
	};

	class texture2d final : public resource {
	public:
		texture2d() = default;
		
		~texture2d() = default;

		DXGI_FORMAT format() const noexcept;
		
		size_t width() const noexcept;

		size_t height() const noexcept;

		size_t alignment() const noexcept;
		
		void upload(
			const graphics_command_list& command_list,
			const buffer& buffer) const;

		static texture2d create(
			const D3D12_RESOURCE_STATES& state,
			const D3D12_HEAP_PROPERTIES& heap,
			const D3D12_RESOURCE_DESC& desc,
			const device& device);
		
		static texture2d default_type(
			const D3D12_RESOURCE_STATES& state,
			const D3D12_RESOURCE_FLAGS& flags,
			const DXGI_FORMAT& format,
			const device& device,
			size_t width, size_t height);
	private:
		size_t mWidth = 0, mHeight = 0;

		DXGI_FORMAT mFormat = DXGI_FORMAT_UNKNOWN;
	};
}
