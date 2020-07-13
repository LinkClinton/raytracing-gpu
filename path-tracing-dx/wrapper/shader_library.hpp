#pragma once

#include "root_signature.hpp"
#include "device.hpp"

namespace path_tracing::dx::wrapper {

	using shader_functions = std::vector<std::wstring>;

	struct shader_association {
		std::vector<std::wstring> functions;
		std::vector<LPCWSTR> pointers;
		
		shader_association() = default;

		shader_association(const std::vector<std::wstring>& functions);

		shader_association(const shader_association& other);

		shader_association(shader_association&& other) noexcept;

		shader_association& operator=(const shader_association& other);
		shader_association& operator=(shader_association&& other) noexcept;
	};

	struct shader_payload_config final : shader_association {
		D3D12_RAYTRACING_SHADER_CONFIG config = {};

		shader_payload_config() = default;

		shader_payload_config(
			const std::vector<std::wstring>& functions,
			size_t max_attribute_size,
			size_t max_payload_size);
	};

	struct shader_root_signature final : shader_association {
		root_signature root_signature;

		shader_root_signature() = default;

		shader_root_signature(const std::vector<std::wstring>& functions, const wrapper::root_signature& root_signature);
	};
	
	class shader_library final {
	public:
		shader_library() = default;

		shader_library(const shader_library& other);

		shader_library(shader_library&& other) noexcept;
		
		~shader_library() = default;

		shader_library& operator=(const shader_library& other);
		shader_library& operator=(shader_library&& other) noexcept;

		D3D12_DXIL_LIBRARY_DESC desc() const noexcept;
		
		static shader_library create(const std::vector<std::wstring>& functions, const std::vector<byte>& byte_code);
	private:
		void internal_build_exports();
		
		std::vector<D3D12_EXPORT_DESC> mExports;
		std::vector<std::wstring> mFunctions;

		std::vector<byte> mByteCode;

		D3D12_DXIL_LIBRARY_DESC mDesc;
	};
	
}
