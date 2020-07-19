#include "application.hpp"

#include "../path-tracing-core/materials/diffuse_material.hpp"
#include "../path-tracing-core/emitters/surface_emitter.hpp"
#include "../path-tracing-core/emitters/point_emitter.hpp"
#include "../path-tracing-core/shapes/sphere.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

using namespace path_tracing::core;
using namespace path_tracing::dx;

int main() {
	const auto width = static_cast<int>(800);
	const auto height = static_cast<int>(800);
	
	auto app = std::make_shared<application>("application-dx", width, height);

	const auto camera = std::make_shared<cameras::camera>(
		perspective_left_hand(glm::radians(45.f), width, height),
		inverse(lookAtLH(
			vector3(0, 0, 20.f),
			vector3(0, 0, 0), 
			vector3(0, 1.f, 0))),
		0.f, 0.f);

	const auto scene = std::make_shared<scenes::scene>();
	
	/*for (size_t x = 0; x < 10; x++) {
		for (size_t y = 0; y < 10; y++) {
			static auto sphere = std::make_shared<class sphere>(1.f);
			
			const auto position_x = -10 + static_cast<float>(x) / 9 * 20;
			const auto position_y = -10 + static_cast<float>(y) / 9 * 20;
			
			scene->add_entity(std::make_shared<entity>(
				std::make_shared<diffuse_material>(vector3(1.f, 0.f, 0.f)),
				nullptr,
				sphere,
				translate(vector3(position_x, position_y, 0))));
		}
	}*/

	const auto sphere = std::make_shared<class sphere>(1.f);
	
	scene->add_entity(std::make_shared<entity>(
		std::make_shared<diffuse_material>(vector3(1.f, 0.f, 0.f)),
		nullptr,
		sphere,
		translate(vector3(5, 0, 0))));

	scene->add_entity(std::make_shared<entity>(
		std::make_shared<diffuse_material>(vector3(0.f, 1.f, 0.f)),
		nullptr,
		sphere,
		translate(vector3(0, 5, 0))));
	
	scene->add_entity(std::make_shared<entity>(
		std::make_shared<diffuse_material>(vector3(1)),
		std::make_shared<surface_emitter>(vector3(100)),
		sphere,
		translate(vector3(0, 0, 25))
		));
	
	app->initialize();
	app->load(camera, scene);
	app->run_loop();
}