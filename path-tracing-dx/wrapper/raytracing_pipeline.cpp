#include "raytracing_pipeline.hpp"

path_tracing::dx::wrapper::hit_group::hit_group(
	const D3D12_HIT_GROUP_TYPE& type, 
	const std::wstring& intersection,
	const std::wstring& closest_hit, 
	const std::wstring& any_hit, 
	const std::wstring& name) :
	type(type), intersection(intersection), closest_hit(closest_hit),
	any_hit(any_hit), name(name)
{
}

ID3D12StateObject* const* path_tracing::dx::wrapper::raytracing_pipeline::get_address_of() const
{
	return mStateObject.GetAddressOf();
}

ID3D12StateObject* path_tracing::dx::wrapper::raytracing_pipeline::operator->() const
{
	return mStateObject.Get();
}

ID3D12StateObject* path_tracing::dx::wrapper::raytracing_pipeline::get() const
{
	return mStateObject.Get();
}

void path_tracing::dx::wrapper::raytracing_pipeline::link_shader_payload_config(const shader_functions& functions,
	size_t max_attribute_size, size_t max_payload_size)
{
	mShaderPayloadConfigs.push_back(shader_payload_config(functions, max_attribute_size, max_payload_size));
}

void path_tracing::dx::wrapper::raytracing_pipeline::link_shader_root_signature(const shader_functions& functions,
	const root_signature& root_signature)
{
	mShaderRootSignatures.push_back(shader_root_signature(functions, root_signature));
}

void path_tracing::dx::wrapper::raytracing_pipeline::add_shader_library(const shader_library& library)
{
	mShaderLibraries.push_back(library);
}

void path_tracing::dx::wrapper::raytracing_pipeline::add_miss_shader(const std::wstring& function)
{
	mMissShaders.push_back(function);
}

void path_tracing::dx::wrapper::raytracing_pipeline::add_hit_group(const hit_group& group)
{
	mHitGroups.push_back(group);
}

void path_tracing::dx::wrapper::raytracing_pipeline::set_root_signature(const root_signature& root_signature)
{
	mRootSignature = root_signature;
}

void path_tracing::dx::wrapper::raytracing_pipeline::set_ray_generation_shader(const std::wstring& function)
{
	mRayGenerationShader = function;
}

void path_tracing::dx::wrapper::raytracing_pipeline::set_max_depth(size_t depth)
{
	mMaxDepth = depth;
}

void path_tracing::dx::wrapper::raytracing_pipeline::build(const device& device)
{
	std::vector<D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION> associations(association_count());
	std::vector<D3D12_STATE_SUBOBJECT> subobjects(subobject_count());

	size_t association_count = 0;
	size_t subobject_count = 0;

	for (size_t index = 0; index < mShaderPayloadConfigs.size(); index++) {
		auto& association = associations[association_count++];
		auto& subobject_config = subobjects[subobject_count++];
		auto& subobject_export = subobjects[subobject_count++];

		subobject_config.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		subobject_config.pDesc = &mShaderPayloadConfigs[index].config;

		association.NumExports = static_cast<UINT>(mShaderPayloadConfigs[index].functions.size());
		association.pSubobjectToAssociate = &subobject_config;
		association.pExports = mShaderPayloadConfigs[index].pointers.data();

		subobject_export.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		subobject_export.pDesc = &association;
	}

	std::vector<D3D12_LOCAL_ROOT_SIGNATURE> local_signatures_desc(mShaderRootSignatures.size());

	for (size_t index = 0; index < mShaderRootSignatures.size(); index++) {
		auto& association = associations[association_count++];
		auto& subobject_signature = subobjects[subobject_count++];
		auto& subobject_export = subobjects[subobject_count++];

		local_signatures_desc[index].pLocalRootSignature = mShaderRootSignatures[index].root_signature.get();

		subobject_signature.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		subobject_signature.pDesc = &local_signatures_desc[index];

		association.NumExports = static_cast<UINT>(mShaderRootSignatures[index].functions.size());
		association.pSubobjectToAssociate = &subobject_signature;
		association.pExports = mShaderRootSignatures[index].pointers.data();

		subobject_export.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		subobject_export.pDesc = &association;
	}

	std::vector<D3D12_DXIL_LIBRARY_DESC> libraries_desc(mShaderLibraries.size());

	for (size_t index = 0; index < mShaderLibraries.size(); index++) {
		auto& subobject = subobjects[subobject_count++];

		libraries_desc[index] = mShaderLibraries[index].desc();

		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
		subobject.pDesc = &libraries_desc[index];
	}

	std::vector<D3D12_HIT_GROUP_DESC> groups_desc(mHitGroups.size());

	for (size_t index = 0; index < mHitGroups.size(); index++) {
		auto& subobject = subobjects[subobject_count++];

		groups_desc[index].Type = mHitGroups[index].type;
		groups_desc[index].IntersectionShaderImport = mHitGroups[index].intersection.c_str();
		groups_desc[index].ClosestHitShaderImport = mHitGroups[index].closest_hit.c_str();
		groups_desc[index].AnyHitShaderImport = mHitGroups[index].any_hit.c_str();

		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		subobject.pDesc = &groups_desc[index];
	}

	D3D12_GLOBAL_ROOT_SIGNATURE global_signature;

	global_signature.pGlobalRootSignature = mRootSignature.get();
	{
		auto& subobject = subobjects[subobject_count++];

		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
		subobject.pDesc = &global_signature;
	}

	D3D12_RAYTRACING_PIPELINE_CONFIG config;

	config.MaxTraceRecursionDepth = static_cast<UINT>(mMaxDepth);
	{
		auto& subobject = subobjects[subobject_count];

		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
		subobject.pDesc = &config;
	}

	D3D12_STATE_OBJECT_DESC desc = {};

	desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	desc.NumSubobjects = static_cast<UINT>(subobjects.size());
	desc.pSubobjects = subobjects.data();

	device->CreateStateObject(&desc, IID_PPV_ARGS(mStateObject.GetAddressOf()));
	mStateObject->QueryInterface(IID_PPV_ARGS(mStateProperties.GetAddressOf()));
}

path_tracing::dx::wrapper::raytracing_pipeline path_tracing::dx::wrapper::raytracing_pipeline::create()
{
	return raytracing_pipeline();
}

size_t path_tracing::dx::wrapper::raytracing_pipeline::association_count() const noexcept
{
	return mShaderPayloadConfigs.size() + mShaderRootSignatures.size();
}

size_t path_tracing::dx::wrapper::raytracing_pipeline::subobject_count() const noexcept
{
	return
		mShaderPayloadConfigs.size() * 2 +
		mShaderRootSignatures.size() * 2 +
		mShaderLibraries.size() +
		mHitGroups.size() + 2;
}
