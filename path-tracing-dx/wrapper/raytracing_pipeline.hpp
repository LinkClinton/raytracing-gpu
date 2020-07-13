#pragma once

#include "shader_library.hpp"
#include "device.hpp"

namespace path_tracing::dx::wrapper {

	struct hit_group {
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
	};
	
	class raytracing_pipeline final {
	public:
		raytracing_pipeline() = default;

		~raytracing_pipeline() = default;

		ID3D12StateObject* const* get_address_of() const;
		ID3D12StateObject* operator->() const;
		ID3D12StateObject* get() const;

		void link_shader_payload_config(const shader_functions& functions,
			size_t max_attribute_size, size_t max_payload_size);

		void link_shader_root_signature(const shader_functions& functions,
			const root_signature& root_signature);
		
		void add_shader_library(const shader_library& library);

		void add_miss_shader(const std::wstring& function);
		
		void add_hit_group(const hit_group& group);

		void set_root_signature(const root_signature& root_signature);

		void set_ray_generation_shader(const std::wstring& function);

		void set_max_depth(size_t depth);
		
		void build(const device& device);

		static raytracing_pipeline create();
	private:
		size_t association_count() const noexcept;
		
		size_t subobject_count() const noexcept;
		
		ComPtr<ID3D12StateObjectProperties> mStateProperties;
		ComPtr<ID3D12StateObject> mStateObject;

		std::vector<shader_payload_config> mShaderPayloadConfigs;
		std::vector<shader_root_signature> mShaderRootSignatures;
		std::vector<shader_library> mShaderLibraries;
		std::vector<hit_group> mHitGroups;

		std::vector<std::wstring> mMissShaders;

		std::wstring mRayGenerationShader;

		root_signature mRootSignature;

		size_t mMaxDepth = 5;
	};
	
}
