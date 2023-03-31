#include "extensions/json/json_scene_loader.hpp"

#include "runtime/runtime_process.hpp"

int main()
{
	raytracing::runtime::runtime_process process;

	raytracing::extensions::json::json_scene_loader::load(process.service(), "./resources/scenes/cornell-box/scene.json");
	
	process.run_loop();
}