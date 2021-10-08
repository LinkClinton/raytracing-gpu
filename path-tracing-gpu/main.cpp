#include "extensions/json/json_scene_loader.hpp"
#include "runtime/runtime_process.hpp"

void load_scene_from_file(const path_tracing::runtime::runtime_process& process, const std::string& filename)
{
	path_tracing::extensions::json::json_scene_loader loader;

	loader.load(process.service(), filename);
}

int main()
{
	auto process = path_tracing::runtime::runtime_process();

	//load_scene_from_file(process, "./resources/scenes/classroom/scene.json");
	//load_scene_from_file(process, "./resources/scenes/yeahright/yeahright.json");
	//load_scene_from_file(process, "./resources/scenes/glass-plate/scene.json");
	load_scene_from_file(process, "./resources/scenes/cornell-box/scene.json");
	//load_scene_from_file(process, "./resources/scenes/ganesha/ganesha.json");

	process.run_loop();
}