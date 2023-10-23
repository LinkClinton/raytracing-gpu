#pragma once

#include "../../interfaces/noncopyable.hpp"
#include "../../types.hpp"

#include "components/texture.hpp"
#include "components/mesh.hpp"

namespace raytracing::runtime
{

	struct runtime_service;
	
}

namespace raytracing::runtime::resources
{
	using namespace raytracing::runtime::resources::components;

	class resource_system final : public noncopyable
	{
	public:
		template <typename Resource>
		void add(const std::string& name, const Resource& resource);

		template <typename Resource>
		void add(const std::string& name, Resource&& resource);

		template <typename Resource>
		void remove(const std::string& name);

		template <typename Resource>
		const Resource& resource(const std::string& name) const;

		template <typename Resource>
		Resource& resource(const std::string& name);

		template <typename Resource>
		bool has(const std::string& name);
	private:
		template <typename Resource>
		mapping<std::string, Resource>& resource_pool();

		mapping<std::string, wrapper::directx12::raytracing_geometry> mRayTracingGeometries;

		mapping<std::string, cpu_texture> mCpuTextures;
		mapping<std::string, gpu_texture> mGpuTextures;
		mapping<std::string, cpu_mesh> mCpuMeshes;
		mapping<std::string, gpu_mesh> mGpuMeshes;
	};

	template <typename Resource>
	void resource_system::add(const std::string& name, const Resource& resource)
	{
		resource_pool<Resource>().insert({ name, resource });
	}

	template <typename Resource>
	void resource_system::add(const std::string& name, Resource&& resource)
	{
		resource_pool<Resource>().insert({ name, resource });
	}

	template <typename Resource>
	void resource_system::remove(const std::string& name)
	{
		resource_pool<Resource>().erase(name);
	}

	template <typename Resource>
	const Resource& resource_system::resource(const std::string& name) const
	{
		return resource_pool<Resource>().at(name);
	}

	template <typename Resource>
	Resource& resource_system::resource(const std::string& name)
	{
		return resource_pool<Resource>().at(name);
	}

	template <typename Resource>
	bool resource_system::has(const std::string& name)
	{
		return resource_pool<Resource>().find(name) != resource_pool<Resource>().end();
	}

	template <>
	inline mapping<std::string, cpu_texture>& resource_system::resource_pool()
	{
		return mCpuTextures;
	}

	template <>
	inline mapping<std::string, gpu_texture>& resource_system::resource_pool()
	{
		return mGpuTextures;
	}

	template <>
	inline mapping<std::string, cpu_mesh>& resource_system::resource_pool()
	{
		return mCpuMeshes;
	}

	template <>
	inline mapping<std::string, gpu_mesh>& resource_system::resource_pool()
	{
		return mGpuMeshes;
	}

	template <>
	inline mapping<std::string, wrapper::directx12::raytracing_geometry>& resource_system::resource_pool()
	{
		return mRayTracingGeometries;
	}

}
