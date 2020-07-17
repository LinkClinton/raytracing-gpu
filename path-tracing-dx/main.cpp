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
		perspective_left_hand(glm::radians(45.f), 1280.f, 720.f),
		inverse(lookAtLH(
			vector3(0, 0, 20.f),
			vector3(0, 0, 0), 
			vector3(0, 1.f, 0))),
		0.f, 0.f);

	const auto scene = std::make_shared<scenes::scene>();

	scene->add_entity(std::make_shared<entity>(
		std::make_shared<diffuse_material>(vector3(1.f, 0.f, 0.f)),
		nullptr,
		std::make_shared<sphere>(2.f),
		transform()));

	scene->add_entity(std::make_shared<entity>(
		nullptr,
		std::make_shared<point_emitter>(vector3(100.0f)),
		nullptr,
		translate(vector3(0, 0, 25))
		));
	
	app->initialize();
	app->load(camera, scene);
	app->run_loop();
}