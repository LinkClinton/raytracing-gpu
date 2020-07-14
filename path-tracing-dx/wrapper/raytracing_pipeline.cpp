#include "raytracing_pipeline.hpp"

path_tracing::dx::wrapper::hit_group::hit_group(
	const D3D12_HIT_GROUP_TYPE& type, 
	const std::wstring& intersection,
	const std::wstring& closest_hit, 
	const std::wstring& any_hit, 
	const std::wstring& name) :
	type(type), intersection(intersection), closest_hit(closest_hit), any_hit(any_hit), name(name)
{
}
