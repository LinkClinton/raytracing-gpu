#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../interfaces/indexable.hpp"
#include "../textures/texture.hpp"
#include "../shared/transform.hpp"

namespace path_tracing::core::materials {

	using namespace shared;
	using namespace textures;
	
	enum class material_type : uint32 {
		unknown = 0,
		diffuse = 1,
		mirror = 2,
		substrate = 3,
		metal = 4
	};
	
	struct material_gpu_buffer {
		constexpr static inline uint32 null = std::numeric_limits<uint32>::max();
		
		material_type type = material_type::unknown;

		uint32 reflectance = null;
		uint32 specular = null;
		uint32 diffuse = null;
		uint32 roughness_u = null;
		uint32 roughness_v = null;
		uint32 eta = null;
		uint32 k = null;
		uint32 remapped = 0;

		vector3 unused = vector3(0);
		
		material_gpu_buffer() = default;
	};

	class material : public interfaces::noncopyable, public interfaces::indexable {
	public:
		explicit material(const material_type& type);

		~material() = default;

		virtual material_gpu_buffer gpu_buffer() const noexcept = 0;

		material_type type() const noexcept;

		static uint32 count() noexcept;
	private:
		static inline uint32 mIndexCount = 0;
		
		material_type mType;
	};
}
