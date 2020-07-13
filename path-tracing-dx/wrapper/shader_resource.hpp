#pragma once

#include "command_list.hpp"

namespace path_tracing::dx::wrapper {

	class shader_resource : public noncopyable {
	public:
		shader_resource() = default;
		
		~shader_resource() = default;

		ID3D12Resource* const* get_address_of() const;
		ID3D12Resource* operator->() const;
		ID3D12Resource* get() const;

		D3D12_RESOURCE_BARRIER barrier(const D3D12_RESOURCE_STATES& before, const D3D12_RESOURCE_STATES& after) const;
	protected:
		ComPtr<ID3D12Resource> mResource = nullptr;
	};

	class buffer final : public shader_resource {
	public:
		explicit buffer(
			const std::shared_ptr<device>& device,
			const D3D12_RESOURCE_STATES& state, 
			const D3D12_RESOURCE_FLAGS& flags, 
			const D3D12_HEAP_TYPE& type, size_t size);

		~buffer() = default;

		size_t size() const noexcept;

		void* map() const;

		void unmap() const;
	private:
		size_t mSize = 0;
	};

	class texture2d final : public shader_resource {
	public:
		explicit texture2d(
			const std::shared_ptr<device>& device,
			const D3D12_RESOURCE_STATES& state,
			const D3D12_RESOURCE_FLAGS& flags,
			const D3D12_HEAP_TYPE& type,
			const DXGI_FORMAT& format,
			size_t width, size_t height);

		explicit texture2d(
			const ComPtr<ID3D12Resource>& resource,
			const DXGI_FORMAT& format,
			size_t width, size_t height);
		
		~texture2d() = default;

		DXGI_FORMAT format() const noexcept;

		size_t width() const noexcept;

		size_t height() const noexcept;

		size_t alignment() const noexcept;

		void upload(
			const std::shared_ptr<graphics_command_list>& command_list,
			const std::shared_ptr<buffer>& buffer) const;
	private:
		size_t mWidth = 0, mHeight = 0;

		DXGI_FORMAT mFormat = DXGI_FORMAT_UNKNOWN;
	};
}
