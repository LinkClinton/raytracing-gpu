#pragma once

#include <directx12-wrapper/resources/texture2d.hpp>

#include "../interfaces/noncopyable.hpp"
#include "../../types.hpp"

namespace path_tracing::runtime {

	struct runtime_service;
	
}

namespace path_tracing::runtime::resources {

	class resource_system final : public noncopyable {
	public:
		resource_system() = default;

		~resource_system() = default;

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

		mapping<std::string, wrapper::directx12::texture2d> mTextures;
	};

	template <typename Resource>
	void resource_system::add(const std::string& name, const Resource& resource)
	{
		resource_pool<Resource>().insert({ name, resource });
	}

	template <typename Resource>
	void resource_system::add(const std::string& name, Resource&& resource)
	{
		resource_pool<Resource>().insert({ name, std::move(resource) });
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
	inline mapping<std::string, wrapper::directx12::texture2d>& resource_system::resource_pool()
	{
		return mTextures;
	}

}
