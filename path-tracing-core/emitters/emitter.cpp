#include "emitter.hpp"

path_tracing::core::emitters::emitter::emitter(const emitter_type& type) :
	indexable(mIndexCount++), mType(type)
{
}

path_tracing::core::emitters::emitter_type path_tracing::core::emitters::emitter::type() const noexcept
{
	return mType;
}

path_tracing::core::uint32 path_tracing::core::emitters::emitter::count() noexcept
{
	return mIndexCount;
}
