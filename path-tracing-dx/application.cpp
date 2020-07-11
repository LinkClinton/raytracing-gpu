#include "application.hpp"

path_tracing::dx::application::application(const std::string& name, int width, int height) :
	application_base(name, width, height)
{
}

path_tracing::dx::application::~application()
{
}

void path_tracing::dx::application::initialize_graphics()
{
}
