#pragma once

#include "root_signature.hpp"
#include "device.hpp"

namespace path_tracing::dx::wrapper {

	struct shader_raytracing_config final {
		size_t max_attribute_size = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;
		size_t max_payload_size = 32;

		shader_raytracing_config() = default;

		shader_raytracing_config(size_t max_attribute_size, size_t max_payload_size);

		size_t hash() const noexcept;
	};
	
	struct shader_association final {
		std::shared_ptr<root_signature> root_signature;
		shader_raytracing_config config;
		
		std::wstring name;
		
		shader_association() = default;

		shader_association(
			const std::shared_ptr<wrapper::root_signature>& root_signature,
			const shader_raytracing_config& config, 
			const std::wstring& name);
	};
	
	class shader_library final : public noncopyable {
	public:
		explicit shader_library(const std::vector<std::wstring>& functions, const std::vector<byte>& code);

		~shader_library() = default;

		D3D12_DXIL_LIBRARY_DESC desc() const;
	private:
		std::vector<D3D12_EXPORT_DESC> mExports;
		std::vector<std::wstring> mFunctions;

		std::vector<byte> mByteCode;

		D3D12_DXIL_LIBRARY_DESC mDesc = {};
	};
	
}
