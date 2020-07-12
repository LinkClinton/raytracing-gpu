#include "renderer_backend.hpp"

path_tracing::core::renderer_backend::renderer_backend(void* handle, int width, int height) :
	mHandle(handle), mWidth(width), mHeight(height)
{
}
