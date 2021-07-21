#include "renderer.hpp"

path_tracing::renderers::renderer::renderer(const runtime_service& service)
{
}

path_tracing::uint32 path_tracing::renderers::renderer::sample_index() const noexcept
{
	return mSampleIndex;
}
