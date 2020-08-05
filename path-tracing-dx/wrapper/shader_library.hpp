#pragma once

#include "root_signature.hpp"
#include "device.hpp"

#include <optional>

namespace path_tracing::dx::wrapper {

	struct shader_raytracing_config final {
		constexpr static inline size_t default_payload_size = 256;
		
		size_t max_attribute_size = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;
		size_t max_payload_size = default_payload_size;

		shader_raytracing_config() = default;

		shader_raytracing_config(size_t max_attribute_size, size_t max_payload_size);

		size_t hash() const noexcept;

		static size_t encode(const shader_raytracing_config& config);

		static shader_raytracing_config decode(size_t code);
	};
	
	struct shader_association final {
		std::shared_ptr<root_signature> root_signature;

		std::optional<shader_raytracing_config> config;
		
		std::wstring name;
		
		shader_association() = default;

		shader_association(
			const std::shared_ptr<wrapper::root_signature>& root_signature,
			const std::optional<shader_raytracing_config>& config, 
			const std::wstring& name);
	};
	
	class shader_library final : public noncopyable {
	public:
		explicit shader_library(const std::vector<std::wstring>& functions, const std::vector<byte>& code);

		~shader_library() = default;

		D3D12_DXIL_LIBRARY_DESC desc() const;

		static std::vector<byte> compile_from_file(const std::wstring& file_name);
	private:
		std::vector<D3D12_EXPORT_DESC> mExports;
		std::vector<std::wstring> mFunctions;

		std::vector<byte> mByteCode;

		D3D12_DXIL_LIBRARY_DESC mDesc = {};
	};
	
}
