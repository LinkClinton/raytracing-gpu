#include "application.hpp"

#include "../path-tracing-core/importers/pbrt_importer.hpp"

#include <memory>

using namespace path_tracing::core;
using namespace path_tracing::dx;

int main() {
	//const auto scene = importers::import_pbrt_scene("./../scenes/cornell-box/scene.pbrt");
	//const auto scene = importers::import_pbrt_scene("./../scenes/dragon/f0-0.pbrt");
	//const auto scene = importers::import_pbrt_scene("./../scenes/basic-test/uv-test.pbrt");
	//const auto scene = importers::import_pbrt_scene("./../scenes/basic-test/uber-test.pbrt");
	const auto scene = importers::import_pbrt_scene("./../scenes/measure-one/frame25.pbrt");
	//const auto scene = importers::import_pbrt_scene("./../scenes/measure-one/frame380.pbrt");
	//const auto scene = importers::import_pbrt_scene("./../scenes/ganesha/ganesha.pbrt");
	//const auto scene = importers::import_pbrt_scene("./../scenes/yeahright/yeahright.pbrt");

	
	const auto width = static_cast<int>(scene.width);
	const auto height = static_cast<int>(scene.height);
	
	auto app = std::make_shared<application>("application-dx", width , height);
	
	app->initialize();
	app->load(scene.camera, scene.scene, { scene.width, scene.height, scene.scale });
	app->run_loop();
}