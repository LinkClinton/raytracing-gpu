#pragma once

#include "shader_resource.hpp"
#include "descriptor_heap.hpp"

namespace path_tracing::dx::wrapper {

	class swap_chain final : public noncopyable {
	public:
		explicit swap_chain(
			const std::shared_ptr<command_queue>& queue,
			const std::shared_ptr<device>& device,
			size_t width, size_t height, void* handle);

		~swap_chain() = default;

		IDXGISwapChain4* const* get_address_of() const;
		IDXGISwapChain4* operator->() const;
		IDXGISwapChain4* get() const;

		void resize(const std::shared_ptr<device>& device, size_t width, size_t height);

		void present(bool sync = true) const;

		std::shared_ptr<texture2d> buffer(size_t index) const;

		size_t current_frame_index() const;
		
		DXGI_FORMAT format() const noexcept;

		size_t count() const noexcept;
	private:
		ComPtr<IDXGISwapChain4> mSwapChain = nullptr;

		DXGI_FORMAT mFormat = DXGI_FORMAT_UNKNOWN;

		std::vector<std::shared_ptr<texture2d>> mBackBuffers;

		size_t mBufferCount = 2;
	};
	
}
