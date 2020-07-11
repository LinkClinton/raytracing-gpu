#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../shared/transform.hpp"

namespace path_tracing::core::materials {

	using namespace shared;
	
	enum class material_type : uint32 {
		unknown = 0,
		diffuse = 1
	};
	
	struct material_gpu_buffer {
		material_type type = material_type::unknown;
		vector3 diffuse = vector3(0);
		
		material_gpu_buffer() = default;
	};

	class material : public interfaces::noncopyable {
	public:
		explicit material(const material_type& type);

		~material() = default;

		virtual material_gpu_buffer gpu_buffer() const noexcept = 0;

		material_type type() const noexcept;
	private:
		material_type mType;
	};
}
