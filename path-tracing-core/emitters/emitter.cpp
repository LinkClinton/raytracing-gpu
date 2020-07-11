#include "emitter.hpp"

path_tracing::core::emitters::emitter::emitter(const emitter_type& type) : mType(type)
{
}

path_tracing::core::emitters::emitter_type path_tracing::core::emitters::emitter::type() const noexcept
{
	return mType;
}
