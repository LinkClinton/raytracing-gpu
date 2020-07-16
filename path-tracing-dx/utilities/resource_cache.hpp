#pragma once

#include "../../path-tracing-core/materials/material.hpp"
#include "../../path-tracing-core/emitters/emitter.hpp"
#include "../../path-tracing-core/scenes/entity.hpp"
#include "../../path-tracing-core/shapes/shape.hpp"

#include "../wrapper/raytracing_acceleration.hpp"
#include "../wrapper/raytracing_pipeline.hpp"
#include "../wrapper/shader_resource.hpp"
#include "../wrapper/shader_library.hpp"

#include <unordered_map>
#include <optional>

namespace path_tracing::dx::utilities {

	using namespace wrapper;

	using namespace core::materials;
	using namespace core::emitters;
	using namespace core::scenes;
	using namespace core::shapes;

	struct shape_cache_data final {
		std::shared_ptr<raytracing_geometry> geometry;
		
		std::shared_ptr<buffer> positions;
		std::shared_ptr<buffer> normals;
		std::shared_ptr<buffer> indices;
		std::shared_ptr<buffer> uvs;

		shape_cache_data() = default;

		shape_cache_data(
			const std::shared_ptr<buffer>& positions,
			const std::shared_ptr<buffer>& normals,
			const std::shared_ptr<buffer>& indices,
			const std::shared_ptr<buffer>& uvs);
	};

	struct entity_cache_data final {
		std::optional<shader_association> association;
		std::optional<raytracing_instance> instance;
		std::optional<material_gpu_buffer> material;
		std::optional<emitter_gpu_buffer> emitter;
		std::optional<hit_group> group;

		std::vector<byte> shader_table_data;
		
		entity_cache_data() = default;

		entity_cache_data(
			const std::optional<shader_association>& association,
			const std::optional<raytracing_instance>& instance,
			const std::optional<material_gpu_buffer>& material,
			const std::optional<emitter_gpu_buffer>& emitter,
			const std::optional<hit_group>& group);
	};

	struct shape_type_data final {
		shader_association association;

		hit_group group;

		shape_type_data() = default;

		shape_type_data(
			const shader_association& association,
			const hit_group& group);
	};
	
	class resource_cache final : public noncopyable {
	public:
		explicit resource_cache(const std::shared_ptr<device>& device);

		~resource_cache() = default;

		void cache_entity(const std::shared_ptr<entity>& entity);
		
		void cache_shape(const std::shared_ptr<shape>& shape);

		void execute(const std::shared_ptr<command_queue>& queue);

		entity_cache_data cache(const std::shared_ptr<entity>& entity) const;
		
		shape_cache_data cache(const std::shared_ptr<shape>& shape) const;

		bool has_cache(const std::shared_ptr<entity>& entity) const noexcept;
		
		bool has_cache(const std::shared_ptr<shape>& shape) const noexcept;
	private:
		std::shared_ptr<buffer>& cpu_buffer();

		std::unordered_map<std::shared_ptr<entity>, entity_cache_data> mEntitiesCache;
		std::unordered_map<std::shared_ptr<shape>, shape_cache_data> mShapesCache;

		std::unordered_map<std::string, shape_type_data> mTypeCache;
		
		std::shared_ptr<command_allocator> mCommandAllocator;
		std::shared_ptr<graphics_command_list> mCommandList;
		
		std::shared_ptr<device> mDevice;
		
		std::vector<std::shared_ptr<buffer>> mCpuBuffers;
	};
	
}
