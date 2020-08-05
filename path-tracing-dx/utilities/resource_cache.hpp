#pragma once

#include "../../path-tracing-core/materials/material.hpp"
#include "../../path-tracing-core/textures/texture.hpp"
#include "../../path-tracing-core/emitters/emitter.hpp"
#include "../../path-tracing-core/scenes/entity.hpp"
#include "../../path-tracing-core/shapes/shape.hpp"

#include "../wrapper/raytracing_acceleration.hpp"
#include "../wrapper/raytracing_pipeline.hpp"
#include "../wrapper/shader_resource.hpp"
#include "../wrapper/shader_library.hpp"

#include <unordered_map>
#include <unordered_set>
#include <optional>

namespace path_tracing::dx::utilities {

	using namespace wrapper;

	using namespace core::materials;
	using namespace core::textures;
	using namespace core::emitters;
	using namespace core::scenes;
	using namespace core::shapes;
	using namespace core;

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
		shader_association association;
		hit_group group;

		std::vector<byte> shader_table_data;

		entity_cache_data() = default;

		entity_cache_data(
			const shader_association& association,
			const hit_group& group);
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

		void execute(
			const std::vector<std::shared_ptr<entity>>& entities, 
			const std::shared_ptr<command_queue>& queue);

		const std::vector<std::shared_ptr<texture2d>>& images_cache_data() const noexcept;

		const std::unordered_set<std::wstring>& shaders_cache_data() const noexcept;

		const std::vector<entity_cache_data>& entities_cache_data() const noexcept;

		const std::vector<shape_cache_data>& shapes_cache_data() const noexcept;

		const std::vector<raytracing_instance>& instances() const noexcept;
		
		const std::vector<material_gpu_buffer>& materials() const noexcept;

		const std::vector<texture_gpu_buffer>& textures() const noexcept;
		
		const std::vector<emitter_gpu_buffer>& emitters() const noexcept;
		
		const std::vector<entity_gpu_buffer>& entities() const noexcept;

		const std::vector<shape_gpu_buffer>& shapes() const noexcept;
	private:
		std::shared_ptr<buffer>& cpu_buffer();

		std::unordered_map<std::shared_ptr<shape>, size_t> mShapesIndex;
		std::unordered_map<std::string, shape_type_data> mTypeCache;

		std::unordered_set<std::wstring> mShadersCache;
		
		std::vector<std::shared_ptr<texture2d>> mImagesCache;
		
		std::vector<shape_cache_data> mShapesCache;
		std::vector<entity_cache_data> mEntitiesCache;
		
		std::shared_ptr<command_allocator> mCommandAllocator;
		std::shared_ptr<graphics_command_list> mCommandList;
		
		std::shared_ptr<device> mDevice;
		
		std::vector<std::shared_ptr<buffer>> mCpuBuffers;

		std::vector<raytracing_instance> mInstances;
		
		std::vector<material_gpu_buffer> mMaterials;
		std::vector<texture_gpu_buffer> mTextures;
		std::vector<emitter_gpu_buffer> mEmitters;
		std::vector<shape_gpu_buffer> mShapes;

		std::vector<entity_gpu_buffer> mEntities;
	};
	
}
