#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../materials/material.hpp"
#include "../emitters/emitter.hpp"
#include "../shapes/shape.hpp"

#include <memory>

namespace path_tracing::core::scenes {

	using namespace materials;
	using namespace emitters;
	using namespace shapes;

	class entity final : public interfaces::noncopyable {
	public:
		explicit entity(
			const std::shared_ptr<material>& material,
			const std::shared_ptr<emitter>& emitter,
			const std::shared_ptr<shape>& shape,
			const transform& transform);

		~entity() = default;

		template <typename T>
		std::shared_ptr<T> component() const noexcept;
		
		template <typename T>
		bool has_component() const noexcept;

		transform transform() const noexcept;
	private:
		std::shared_ptr<material> mMaterial;
		std::shared_ptr<emitter> mEmitter;
		std::shared_ptr<shape> mShape;

		shared::transform mTransform;
	};

	template <>
	inline std::shared_ptr<material> entity::component() const noexcept
	{
		return mMaterial;
	}

	template <>
	inline std::shared_ptr<emitter> entity::component() const noexcept
	{
		return mEmitter;
	}

	template <>
	inline std::shared_ptr<shape> entity::component() const noexcept
	{
		return mShape;
	}

	template <typename T>
	bool entity::has_component() const noexcept
	{
		return component<T>() != nullptr;
	}


}
