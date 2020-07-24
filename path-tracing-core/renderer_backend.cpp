#include "renderer_backend.hpp"

path_tracing::core::render_config::render_config(size_t width, size_t height, real scale) :
	width(width), height(height), scale(scale)
{
}

path_tracing::core::renderer_backend::renderer_backend(void* handle, int width, int height) :
	mHandle(handle), mWidth(width), mHeight(height)
{
}
