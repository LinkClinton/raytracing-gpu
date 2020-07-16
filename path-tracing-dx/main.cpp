#include "application.hpp"

#include "../path-tracing-core/materials/diffuse_material.hpp"
#include "../path-tracing-core/emitters/point_emitter.hpp"
#include "../path-tracing-core/shapes/sphere.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

using namespace path_tracing::core;
using namespace path_tracing::dx;

int main() {
	auto app = std::make_shared<application>("application-dx", 1280, 720);

	const auto camera = std::make_shared<cameras::camera>(
		glm::perspectiveFovLH(glm::radians(45.f), 1280.f, 720.f, 0.f, 1000.f),
		inverse(lookAtLH(
			vector3(0, 0, 20.f),
			vector3(0, 0, 0), 
			vector3(0, 1.f, 0))),
		0.f, 0.f);

	const auto scene = std::make_shared<scenes::scene>();

	scene->add_entity(std::make_shared<entity>(
		std::make_shared<diffuse_material>(vector3(1.f)),
		std::make_shared<point_emitter>(vector3(1)),
		std::make_shared<sphere>(1.f),
		transform()));
	
	app->initialize();
	app->load(camera, scene);
	app->run_loop();
}