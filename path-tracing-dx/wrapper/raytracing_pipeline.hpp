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
	};
	
	class raytracing_pipeline final : public noncopyable {
	public:
		explicit raytracing_pipeline() = default;
	private:
		ComPtr<ID3D12StateObjectProperties> mStateProperties;
		ComPtr<ID3D12StateObject> mStateObject;

		std::shared_ptr<root_signature> mRootSignature;

		std::vector<shader_association> mShaderAssociations;
		std::vector<shader_library> mShaderLibraries;
		std::vector<hit_group> mHitGroupShaders;

		std::vector<std::wstring> mMissShaders;

		std::wstring mRayGenerationShader;

		size_t mMaxDepth = 5;
	};
	
}
