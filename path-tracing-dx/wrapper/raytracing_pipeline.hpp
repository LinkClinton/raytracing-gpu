#pragma once

#include "shader_table.hpp"
#include "device.hpp"

namespace path_tracing::dx::wrapper {
	
	struct hit_group final {
		D3D12_HIT_GROUP_TYPE type = D3D12_HIT_GROUP_TYPE_TRIANGLES;

		std::wstring intersection;
		std::wstring closest_hit;
		std::wstring any_hit;
		std::wstring name;

		hit_group() = default;

		hit_group(
			const D3D12_HIT_GROUP_TYPE& type,
			const std::wstring& intersection,
			const std::wstring& closest_hit,
			const std::wstring& any_hit,
			const std::wstring& name);

		LPCWSTR intersection_export() const noexcept;
		LPCWSTR closest_hit_export() const noexcept;
		LPCWSTR any_hit_export() const noexcept;
	};
	
	class raytracing_pipeline final : public noncopyable {
	public:
		raytracing_pipeline() = default;

		~raytracing_pipeline() = default;

		ID3D12StateObject* const* get_address_of() const;
		ID3D12StateObject* operator->() const;
		ID3D12StateObject* get() const;
		
		void add_shader_library(const std::shared_ptr<shader_library>& library);
		
		void add_shader_association(const shader_association& association);

		void add_hit_group_shader(const hit_group& hit_group);

		void add_miss_shader(const std::wstring& name);

		void set_root_signature(const std::shared_ptr<root_signature>& root_signature);

		void set_ray_generation_shader(const std::wstring& name);

		void set_max_depth(size_t depth);

		void build(const std::shared_ptr<device>& device);
		
		std::shared_ptr<shader_table> shader_table() const noexcept;

		const std::vector<hit_group>& hit_group_shaders() const noexcept;
		
		const std::vector<std::wstring>& miss_shaders() const noexcept;
		
		const std::wstring& ray_generation_shader() const noexcept;
	private:
		size_t compute_association_count() const;
		
		size_t compute_subobject_count() const;
	private:
		ComPtr<ID3D12StateObjectProperties> mStateProperties;
		ComPtr<ID3D12StateObject> mStateObject;

		std::shared_ptr<wrapper::root_signature> mRootSignature;
		std::shared_ptr<wrapper::shader_table> mShaderTable;

		std::unordered_map<std::shared_ptr<root_signature>, shader_functions> mShaderSignatureAssociations;
		std::unordered_map<size_t, shader_functions> mShaderConfigAssociations;
		
		std::vector<std::shared_ptr<shader_library>> mShaderLibraries;
		std::vector<shader_association> mShaderAssociations;
		std::vector<hit_group> mHitGroupShaders;

		std::vector<std::wstring> mMissShaders;

		std::wstring mRayGenerationShader;

		size_t mMaxDepth = 5;
	};
	
}
