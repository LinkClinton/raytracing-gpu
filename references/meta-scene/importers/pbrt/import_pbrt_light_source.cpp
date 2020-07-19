#include "import_pbrt_light_source.hpp"
#include "import_pbrt_spectrum.hpp"

#include "../../emitters/environment_emitter.hpp"
#include "../../emitters/directional_emitter.hpp"
#include "../../emitters/surface_emitter.hpp"
#include "../../logs.hpp"

#include <filesystem>

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_infinite_light(scene_context& context, std::shared_ptr<emitter>& emitter)
	{
		auto instance = std::make_shared<environment_emitter>();

		std::shared_ptr<spectrum> intensity = std::make_shared<color_spectrum>(static_cast<real>(1));
		std::shared_ptr<spectrum> scale = std::make_shared<color_spectrum>(static_cast<real>(1));

		instance->intensity = std::make_shared<color_spectrum>();
		instance->environment_map = "";
		instance->gamma = false;
		
		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_STRING_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "mapname") META_SCENE_FINISHED_AND_RETURN(instance->environment_map = context.directory_path + value);
				}

				if (type == PBRT_INTEGER_TOKEN) {
					const auto value = context.peek_integer<size_t>();

					if (name == "nsamples") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : samples is not support in infinte light."));
				}
			
				if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
					const auto value = context.peek_one_token();
					
					if (name == "scale") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum(value, scale));
					if (name == "L") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum(value, intensity));
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		const auto extension = std::filesystem::path(instance->environment_map).extension().string();

		if (extension != ".hdr" && extension != ".exr") instance->gamma = true;
		
		instance->intensity = multiply_spectrum(intensity, scale);

		emitter = instance;
	}

	void import_area_light(scene_context& context, std::shared_ptr<emitter>& emitter)
	{
		auto instance = std::make_shared<surface_emitter>();

		std::shared_ptr<spectrum> intensity = std::make_shared<color_spectrum>(static_cast<real>(1));
		std::shared_ptr<spectrum> scale = std::make_shared<color_spectrum>(static_cast<real>(1));
		
		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
					const auto value = context.peek_one_token();

					if (name == "scale") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum(value, scale));
					if (name == "L") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum(value, intensity));
				}

				if (type == PBRT_BLACK_BODY_TOKEN) {
					const auto value = context.peek_one_token();
					
					if (name == "L") META_SCENE_FINISHED_AND_RETURN(import_black_body_spectrum(value, intensity));
				}

				if (type == PBRT_INTEGER_TOKEN) {
					const auto value = context.peek_integer<size_t>();

					if (name == "nsamples") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : samples is not support in area light."));
				}

				if (type == PBRT_SPECTRUM_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "L") META_SCENE_FINISHED_AND_RETURN(import_sampled_spectrum(context.directory_path + value, intensity));
				}
			
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		instance->radiance = multiply_spectrum(intensity, scale);
		
		emitter = instance;
	}

	void import_distant_light(scene_context& context, std::shared_ptr<emitter>& emitter)
	{
		auto instance = std::make_shared<directional_emitter>();

		instance->irradiance = std::make_shared<color_spectrum>();
		instance->from = vector3(0, 0, 0);
		instance->to = vector3(0, 0, 1);

		std::shared_ptr<spectrum> irradiance = std::make_shared<color_spectrum>(static_cast<real>(1));
		std::shared_ptr<spectrum> scale = std::make_shared<color_spectrum>(static_cast<real>(1));
		
		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
					const auto value = context.peek_one_token();

					if (name == "scale") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum(value, scale));
					if (name == "L") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum(value, irradiance));
				}

				if (type == PBRT_POINT_TOKEN) {
					const auto value = context.peek_one_token();

					if (name == "from") META_SCENE_FINISHED_AND_RETURN(import_token_vector3(value, instance->from));
					if (name == "to") META_SCENE_FINISHED_AND_RETURN(import_token_vector3(value, instance->to));
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		instance->irradiance = multiply_spectrum(irradiance, scale);
		
		emitter = instance;
	}

	void import_light_source(scene_context& context)
	{
		// the first token should be the type of light source
		const auto type = remove_special_character(context.peek_one_token());

		const auto entity = std::make_shared<metascene::entity>();
		
		if (type == "infinite") import_infinite_light(context, entity->emitter);
		if (type == "distant") import_distant_light(context, entity->emitter);
		
		entity->transform = context.current().transform;

		context.scene->entities.push_back(entity);

		META_SCENE_IMPORT_SUCCESS_CHECK(entity->emitter);
	}

	void import_area_light_source(scene_context& context, std::shared_ptr<emitter>& emitter)
	{
		const auto type = remove_special_character(context.peek_one_token());

		std::shared_ptr<emitters::emitter> instance = nullptr;
		
		if (type == "diffuse") import_area_light(context, instance);

		META_SCENE_IMPORT_SUCCESS_CHECK(instance);

		emitter = instance;
	}
}

#endif