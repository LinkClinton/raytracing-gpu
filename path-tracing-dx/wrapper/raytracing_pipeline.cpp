#include "raytracing_pipeline.hpp"

#include <cassert>

path_tracing::dx::wrapper::hit_group::hit_group(
	const D3D12_HIT_GROUP_TYPE& type, 
	const std::wstring& intersection,
	const std::wstring& closest_hit, 
	const std::wstring& any_hit, 
	const std::wstring& name) :
	type(type), intersection(intersection), closest_hit(closest_hit), any_hit(any_hit), name(name)
{
}

LPCWSTR path_tracing::dx::wrapper::hit_group::intersection_export() const noexcept
{
	return intersection.empty() ? nullptr : intersection.c_str();
}

LPCWSTR path_tracing::dx::wrapper::hit_group::closest_hit_export() const noexcept
{
	return closest_hit.empty() ? nullptr : closest_hit.c_str();
}

LPCWSTR path_tracing::dx::wrapper::hit_group::any_hit_export() const noexcept
{
	return any_hit.empty() ? nullptr : any_hit.c_str();
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

void path_tracing::dx::wrapper::raytracing_pipeline::set_shader_libraries(const std::vector<std::shared_ptr<shader_library>>& libraries)
{
	mShaderLibraries = libraries;
}

void path_tracing::dx::wrapper::raytracing_pipeline::set_shader_associations(const std::vector<shader_association>& associations)
{
	mShaderAssociations = associations;
}

void path_tracing::dx::wrapper::raytracing_pipeline::set_hit_group_shaders(const std::vector<hit_group>& hit_groups)
{
	mHitGroupShaders = hit_groups;
}

void path_tracing::dx::wrapper::raytracing_pipeline::set_miss_shaders(const std::vector<std::wstring>& shaders)
{
	mMissShaders = shaders;
}

void path_tracing::dx::wrapper::raytracing_pipeline::set_root_signature(const std::shared_ptr<root_signature>& root_signature)
{
	mRootSignature = root_signature;
}

void path_tracing::dx::wrapper::raytracing_pipeline::set_ray_generation_shader(const std::wstring& name)
{
	mRayGenerationShader = name;
}

void path_tracing::dx::wrapper::raytracing_pipeline::set_max_depth(size_t depth)
{
	mMaxDepth = depth;
}

void path_tracing::dx::wrapper::raytracing_pipeline::build(const std::shared_ptr<device>& device)
{
	// cache the functions that have same root signature or have same shader config
	for (const auto& association : mShaderAssociations) {
		if (association.root_signature != nullptr)
			mShaderSignatureAssociations[association.root_signature].functions.push_back(association.name);

		if (association.config.has_value())
			mShaderConfigAssociations[association.config->hash()].functions.push_back(association.name);
	}

	// rebuild the shader functions(because we need the pointer of shaders)
	for (auto& association : mShaderSignatureAssociations)
		association.second = shader_functions(association.second.functions);

	for (auto& association : mShaderConfigAssociations)
		association.second = shader_functions(association.second.functions);

	std::vector<D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION> associations(compute_association_count());
	std::vector<D3D12_STATE_SUBOBJECT> subobjects(compute_subobject_count());

	size_t association_count = 0;
	size_t subobject_count = 0;
	
	std::vector<D3D12_RAYTRACING_SHADER_CONFIG> configs(mShaderAssociations.size());

	auto config_association = mShaderConfigAssociations.begin();
	
	for (size_t index = 0; index < mShaderConfigAssociations.size(); index++) {
		const auto config = shader_raytracing_config::decode(config_association->first);

		auto& association = associations[association_count++];
		auto& subobject_config = subobjects[subobject_count++];
		auto& subobject_export = subobjects[subobject_count++];

		configs[index].MaxAttributeSizeInBytes = static_cast<UINT>(config.max_attribute_size);
		configs[index].MaxPayloadSizeInBytes = static_cast<UINT>(config.max_payload_size);
		
		subobject_config.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		subobject_config.pDesc = &configs[index];

		association.NumExports = static_cast<UINT>(config_association->second.pointers.size());
		association.pSubobjectToAssociate = &subobject_config;
		association.pExports = config_association->second.pointers.data();

		subobject_export.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		subobject_export.pDesc = &association;

		++config_association;
	}

	std::vector<D3D12_LOCAL_ROOT_SIGNATURE> local_signatures(mShaderSignatureAssociations.size());

	auto signature_association = mShaderSignatureAssociations.begin();

	for (size_t index = 0; index < mShaderSignatureAssociations.size(); index++) {
		auto& association = associations[association_count++];
		auto& subobject_signature = subobjects[subobject_count++];
		auto& subobject_export = subobjects[subobject_count++];

		local_signatures[index].pLocalRootSignature = signature_association->first == nullptr ? nullptr : signature_association->first->get();

		subobject_signature.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		subobject_signature.pDesc = &local_signatures[index];

		association.NumExports = static_cast<UINT>(signature_association->second.pointers.size());
		association.pSubobjectToAssociate = &subobject_signature;
		association.pExports = signature_association->second.pointers.data();

		subobject_export.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		subobject_export.pDesc = &association;

		++signature_association;
	}

	std::vector<D3D12_DXIL_LIBRARY_DESC> libraries(mShaderLibraries.size());

	for (size_t index = 0; index < mShaderLibraries.size(); index++) {
		auto& subobject = subobjects[subobject_count++];

		libraries[index] = mShaderLibraries[index]->desc();
		
		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
		subobject.pDesc = &libraries[index];
	}

	std::vector<D3D12_HIT_GROUP_DESC> groups(mHitGroupShaders.size());
	std::vector<std::wstring> group_shaders(mHitGroupShaders.size());
	
	for (size_t index = 0; index < mHitGroupShaders.size(); index++) {
		auto& subobject = subobjects[subobject_count++];

		groups[index].Type = mHitGroupShaders[index].type;
		groups[index].IntersectionShaderImport = mHitGroupShaders[index].intersection_export();
		groups[index].ClosestHitShaderImport = mHitGroupShaders[index].closest_hit_export();
		groups[index].AnyHitShaderImport = mHitGroupShaders[index].any_hit_export();
		groups[index].HitGroupExport = mHitGroupShaders[index].name.c_str();

		group_shaders[index] = mHitGroupShaders[index].name;
		
		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		subobject.pDesc = &groups[index];
	}

	D3D12_GLOBAL_ROOT_SIGNATURE global_root_signature;
	
	if (mRootSignature != nullptr) {
		auto& subobject = subobjects[subobject_count++];

		global_root_signature.pGlobalRootSignature = mRootSignature->get();

		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
		subobject.pDesc = &global_root_signature;
	}

	D3D12_RAYTRACING_PIPELINE_CONFIG config;
	{
		auto& subobject = subobjects[subobject_count++];

		config.MaxTraceRecursionDepth = static_cast<UINT>(mMaxDepth);
		
		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
		subobject.pDesc = &config;
	}

	assert(association_count == associations.size());
	assert(subobject_count == subobjects.size());
	
	D3D12_STATE_OBJECT_DESC desc;

	desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	desc.NumSubobjects = static_cast<UINT>(subobjects.size());
	desc.pSubobjects = subobjects.data();

	(*device)->CreateStateObject(&desc, IID_PPV_ARGS(mStateObject.GetAddressOf()));
	mStateObject->QueryInterface(IID_PPV_ARGS(mStateProperties.GetAddressOf()));

	mShaderTable = std::make_shared<wrapper::shader_table>(device, mShaderAssociations, group_shaders, mMissShaders, mRayGenerationShader);

	mShaderTable->copy_shader_identifier_from(mStateProperties);
}

std::shared_ptr<path_tracing::dx::wrapper::shader_table> path_tracing::dx::wrapper::raytracing_pipeline::shader_table() const noexcept
{
	return mShaderTable;
}

const std::vector<path_tracing::dx::wrapper::hit_group>& path_tracing::dx::wrapper::raytracing_pipeline::hit_group_shaders() const noexcept
{
	return mHitGroupShaders;
}

const std::vector<std::wstring>& path_tracing::dx::wrapper::raytracing_pipeline::miss_shaders() const noexcept
{
	return mMissShaders;
}

const std::wstring& path_tracing::dx::wrapper::raytracing_pipeline::ray_generation_shader() const noexcept
{
	return mRayGenerationShader;
}

size_t path_tracing::dx::wrapper::raytracing_pipeline::compute_association_count() const
{
	return mShaderSignatureAssociations.size() + mShaderConfigAssociations.size();
}

size_t path_tracing::dx::wrapper::raytracing_pipeline::compute_subobject_count() const
{
	return
		mShaderSignatureAssociations.size() * 2 +
		mShaderConfigAssociations.size() * 2 +
		mShaderLibraries.size() +
		mHitGroupShaders.size() + 1 +
		(mRootSignature == nullptr ? 0 : 1);
}
