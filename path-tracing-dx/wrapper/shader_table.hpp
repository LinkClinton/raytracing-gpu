#pragma once

#include "shader_resource.hpp"
#include "shader_library.hpp"

namespace path_tracing::dx::wrapper {

	struct shader_functions {
		std::vector<std::wstring> functions;
		std::vector<LPCWSTR> pointers;

		shader_functions() = default;

		shader_functions(const shader_functions& other);

		shader_functions(shader_functions&& other) noexcept;

		shader_functions(const std::vector<std::wstring>& functions);

		shader_functions& operator=(const shader_functions& other);
		shader_functions& operator=(shader_functions&& other) noexcept;
	};
	
	struct shader_record final {
		size_t address = 0;
		size_t size = 0;

		shader_record() = default;

		shader_record(size_t address, size_t size);
	};
	
	class shader_table final : public noncopyable {
	public:
		explicit shader_table(
			const std::shared_ptr<device>& device,
			const std::vector<shader_association>& associations,
			const std::vector<std::wstring>& hit_group_shaders,
			const std::vector<std::wstring>& miss_shaders,
			const std::wstring& ray_generation_shader);

		~shader_table() = default;

		void copy_shader_identifier_from(const ComPtr<ID3D12StateObjectProperties>& properties);
		
		void begin_mapping();

		void end_mapping();
		
		void upload(const std::shared_ptr<graphics_command_list>& command_list) const;
		
		byte* shader_record_address(const std::wstring& name) const;

		shader_record ray_generation_shader() const noexcept;

		shader_record hit_group_shaders() const noexcept;

		shader_record miss_shaders() const noexcept;
	private:
		std::unordered_map<std::wstring, shader_record> mShaderRecords;
		
		std::shared_ptr<buffer> mCpuBuffer;
		std::shared_ptr<buffer> mGpuBuffer;

		shader_record mRayGenerationShader;
		shader_record mHitGroupShaders;
		shader_record mMissShaders;
		
		byte* mBufferMapping = nullptr;
	};
	
}
