#pragma once

#include "shader_resource.hpp"
#include "shader_library.hpp"

namespace path_tracing::dx::wrapper {

	using shader_functions = std::vector<std::wstring>;

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

		void begin_mapping();

		void end_mapping();

		void upload(const std::shared_ptr<graphics_command_list>& command_list) const;
		
		byte* shader_record_address(const std::wstring& name) const;

		size_t ray_generation_shader_size() const noexcept;

		size_t hit_group_shader_size() const noexcept;

		size_t miss_shader_size() const noexcept;
	private:
		std::unordered_map<std::shared_ptr<root_signature>, shader_functions> mRootSignatureAssociations;
		std::unordered_map<size_t, shader_functions> mShaderRaytracingConfigAssociations;
		std::unordered_map<std::wstring, shader_record> mShaderRecords;
		
		std::shared_ptr<buffer> mCpuBuffer;
		std::shared_ptr<buffer> mGpuBuffer;

		size_t mRayGenerationShaderSize = 0;
		size_t mHitGroupShaderSize = 0;
		size_t mMissShaderSize = 0;

		byte* mBufferMapping = nullptr;
	};
	
}
