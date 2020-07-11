#include "application.hpp"

#include <memory>

using namespace path_tracing::dx;

int main() {
	auto app = std::make_shared<application>("application-dx", 1280, 720);

	app->initialize();
	app->run_loop();
}