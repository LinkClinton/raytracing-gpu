#include "renderer_scene.hpp"

#include "converter/convert_camera.hpp"
#include "converter/convert_scene.hpp"

path_tracing::core::renderer_scene path_tracing::core::create_from_meta_scene(const std::shared_ptr<metascene::scene>& meta_scene)
{
	renderer_scene scene;

	scene.camera = converter::create_camera(meta_scene->camera, meta_scene->film);
	scene.scene = converter::create_scene(meta_scene);
	scene.width = meta_scene->film->width;
	scene.height = meta_scene->film->height;
	scene.scale = meta_scene->film->scale;

	return scene;
}
