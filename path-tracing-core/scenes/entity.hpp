#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../interfaces/indexable.hpp"
#include "../materials/material.hpp"
#include "../emitters/emitter.hpp"
#include "../shapes/shape.hpp"

#include <memory>

namespace path_tracing::core::scenes {

	using namespace materials;
	using namespace emitters;
	using namespace shapes;

	struct entity_gpu_buffer {
		constexpr static inline uint32 null = std::numeric_limits<uint32>::max();
		
		matrix4x4 local_to_world = matrix4x4(1);
		matrix4x4 world_to_local = matrix4x4(1);

		uint32 material = null;
		uint32 emitter = null;
		uint32 shape = null;
		real area = 0;

		entity_gpu_buffer() = default;
	};
	
	class entity final : public interfaces::noncopyable, public interfaces::indexable {
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

		entity_gpu_buffer gpu_buffer() const noexcept;

		static uint32 count() noexcept;
	private:
		static inline uint32 mIndexCount = 0;
		
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
