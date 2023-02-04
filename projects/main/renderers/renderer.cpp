#include "renderer.hpp"

raytracing::renderers::renderer::renderer(const runtime_service& service)
{
}

raytracing::uint32 raytracing::renderers::renderer::sample_index() const noexcept
{
	return mSampleIndex;
}
