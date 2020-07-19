#include "import_pbrt_camera.hpp"

#include "../../cameras/perspective_camera.hpp"
#include "../../samplers/random_sampler.hpp"
#include "../../filters/gaussian_filter.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_film(scene_context& context, std::shared_ptr<film>& film)
	{
		film = std::make_shared<metascene::film>();
		// the first token should be the name of film, but we do not need it
		// so we just peek it without processing
		context.peek_one_token();

		film->filter = std::make_shared<box_filter>();
		film->scale = static_cast<real>(1);
		
		context.loop_important_token([&]
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_INTEGER_TOKEN) {
					const auto value = context.peek_integer<size_t>();

					if (name == "xresolution") META_SCENE_FINISHED_AND_RETURN(film->width = value);
					if (name == "yresolution") META_SCENE_FINISHED_AND_RETURN(film->height = value);
				}

				// because we do not process the "string filename" so we just remove the value 
				if (type == PBRT_STRING_TOKEN) META_SCENE_FINISHED_AND_RETURN(context.peek_one_token());

				// scale property 
				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "scale") META_SCENE_FINISHED_AND_RETURN(film->scale = value);
					if (name == "maxsampleluminance") return;
				}
				
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});
	}

	void import_sampler(scene_context& context, std::shared_ptr<sampler>& sampler)
	{
		sampler = std::make_shared<random_sampler>();
		
		// the first token should be the type of sampler
		// but we only support random sampler now
		const auto sampler_type = remove_special_character(context.peek_one_token());

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_INTEGER_TOKEN) {
					const auto value = context.peek_integer<size_t>();
					
					if (name == "pixelsamples") META_SCENE_FINISHED_AND_RETURN(sampler->sample_per_pixel = value);
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});
	}

	void import_gaussian_filter(scene_context& context, std::shared_ptr<filter>& filter)
	{
		const auto instance = std::make_shared<gaussian_filter>();

		instance->radius_x = 2;
		instance->radius_y = 2;
		instance->alpha = 2;

		context.loop_important_token([&]()
			{
			});
		
		filter = instance;
	}
	
	void import_filter(scene_context& context, std::shared_ptr<filter>& filter)
	{
		const auto name = remove_special_character(context.peek_one_token());

		if (name == "gaussian") import_gaussian_filter(context, filter);

		META_SCENE_IMPORT_SUCCESS_CHECK(filter);
	}

	void import_perspective_camera(scene_context& context, std::shared_ptr<camera>& camera)
	{
		auto instance = std::make_shared<perspective_camera>();

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();
					
					if (name == "fov") META_SCENE_FINISHED_AND_RETURN(instance->fov = value);
					if (name == "focaldistance") META_SCENE_FINISHED_AND_RETURN(instance->focus = value);
					if (name == "lensradius") META_SCENE_FINISHED_AND_RETURN(instance->lens = value);
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});
		
		camera = instance;
	}

	void import_camera(scene_context& context, std::shared_ptr<camera>& camera, const matrix4x4& transform)
	{
		const auto type = remove_special_character(context.peek_one_token());

		std::shared_ptr<cameras::camera> instance;
		
		if (type == "perspective") import_perspective_camera(context, instance);

		instance->transform = transform;

		context.state.transforms.insert({ "camera", transform });
		
		META_SCENE_IMPORT_SUCCESS_CHECK(instance);

		camera = instance;
	}
}

#endif