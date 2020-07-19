#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../shared/transform.hpp"

namespace path_tracing::core::emitters {

	using namespace shared;
	
	enum class emitter_type : uint32 {
		unknown = 0,
		point = 1,
		surface = 2
	};

	struct emitter_gpu_buffer {
		emitter_type type = emitter_type::unknown;
		vector3 intensity = vector3(0);
		
		vector3 position = vector3(0);
		uint32 delta = 0;

		uint32 index = 0;
		vector3 unused = vector3();

		emitter_gpu_buffer() = default;
	};
	
	class emitter : public interfaces::noncopyable {
	public:
		explicit emitter(const emitter_type& type);

		~emitter() = default;

		virtual emitter_gpu_buffer gpu_buffer(const transform& transform, size_t index) const noexcept = 0;

		emitter_type type() const noexcept;
	private:
		emitter_type mType;
	};
}
