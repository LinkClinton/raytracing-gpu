#include "convert_camera.hpp"

#include "meta-scene/cameras/perspective_camera.hpp"

namespace path_tracing::core::converter {

	std::shared_ptr<camera> create_perspective_camera(
		const std::shared_ptr<metascene::cameras::perspective_camera>& camera,
		const std::shared_ptr<metascene::cameras::film>& film)
	{
		const auto width = static_cast<real>(film->width);
		const auto height = static_cast<real>(film->height);
		const auto fov = glm::radians(camera->fov);

		const auto projective =
			shared::perspective_left_hand(fov, width, height, camera->near, camera->far);
		
		return std::make_shared<cameras::camera>(projective, camera->transform, camera->focus, camera->lens);
	}

	std::shared_ptr<camera> create_camera(
		const std::shared_ptr<metascene::cameras::camera>& camera,
		const std::shared_ptr<metascene::cameras::film>& film)
	{
		if (camera->type == metascene::cameras::type::perspective)
			return create_perspective_camera(std::static_pointer_cast<metascene::cameras::perspective_camera>(camera), film);

		return nullptr;
	}
	
}
