#pragma once

#include "shader_resource.hpp"
#include "descriptor_heap.hpp"

namespace path_tracing::dx::wrapper {

	class swap_chain final {
	public:
		swap_chain() = default;

		~swap_chain() = default;

		IDXGISwapChain4* const* get_address_of() const;
		IDXGISwapChain4* operator->() const;
		IDXGISwapChain4* get() const;

		void resize(const device& device, int width, int height);

		void present(bool sync = true) const;

		D3D12_CPU_DESCRIPTOR_HANDLE render_target_view(size_t index) const;

		texture2d buffer(size_t index) const;

		DXGI_FORMAT format() const noexcept;

		size_t count() const noexcept;

		static swap_chain create(const device& device, const command_queue& queue,
			int width, int height, void* handle);
	private:
		ComPtr<IDXGISwapChain4> mSwapChain = nullptr;

		DXGI_FORMAT mFormat = DXGI_FORMAT_UNKNOWN;

		std::vector<texture2d> mBackBuffers;

		descriptor_heap mHeap;

		size_t mDescriptorOffset = 0;
		size_t mBufferCount = 2;
	};
	
}
