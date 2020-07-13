#pragma once

#include "command_list.hpp"

namespace path_tracing::dx::wrapper {

	class shader_resource {
	public:
		shader_resource() = default;

		~shader_resource() = default;

		ID3D12Resource* const* get_address_of() const;
		ID3D12Resource* operator->() const;
		ID3D12Resource* get() const;

		static D3D12_RESOURCE_BARRIER barrier_transition(
			const D3D12_RESOURCE_STATES& before,
			const D3D12_RESOURCE_STATES& after,
			const shader_resource& resource);
	protected:
		ComPtr<ID3D12Resource> mResource = nullptr;
	};

	class buffer final : public shader_resource {
	public:
		buffer() = default;

		~buffer() = default;

		size_t size() const noexcept;

		void* map() const;

		void unmap() const;

		static buffer create(const D3D12_RESOURCE_STATES& state, const D3D12_HEAP_PROPERTIES& heap,
			const D3D12_RESOURCE_DESC& desc, const device& device);
	private:
		size_t mSize = 0;
	};

	class texture2d final : public shader_resource {
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

		static texture2d create(
			ComPtr<ID3D12Resource>& resource,
			const DXGI_FORMAT& format, size_t width, size_t height);
	private:
		size_t mWidth = 0, mHeight = 0;

		DXGI_FORMAT mFormat = DXGI_FORMAT_UNKNOWN;
	};
}
