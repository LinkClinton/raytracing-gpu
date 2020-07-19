#include "import_pbrt_medium.hpp"

#include "../../media/heterogeneous_medium.hpp"
#include "../../media/homogeneous_medium.hpp"
#include "../../logs.hpp"

#include "import_pbrt_spectrum.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_homogeneous_medium(scene_context& context, const property_group& properties, std::shared_ptr<medium>& medium)
	{
		auto instance = std::make_shared<homogeneous_medium>();

		instance->sigma_a = std::make_shared<color_spectrum>(static_cast<real>(0.0011), static_cast<real>(0.0024), static_cast<real>(0.014));
		instance->sigma_s = std::make_shared<color_spectrum>(static_cast<real>(2.55), static_cast<real>(3.21), static_cast<real>(3.77));
		instance->g = 0;

		auto scale = std::make_shared<color_spectrum>(static_cast<real>(1));

		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "sigma_a") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum(value, instance->sigma_a));
				if (name == "sigma_s") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum(value, instance->sigma_s));
			}

			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = string_to_real(remove_special_character(property.second));

				if (name == "g") META_SCENE_FINISHED_AND_CONTINUE(instance->g = value);
				if (name == "scale") META_SCENE_FINISHED_AND_CONTINUE(scale = std::make_shared<color_spectrum>(value));
			}

			if (type == PBRT_STRING_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "type") continue;
			}
		}

		instance->sigma_s = multiply_spectrum(instance->sigma_s, scale);
		instance->sigma_a = multiply_spectrum(instance->sigma_a, scale);

		medium = instance;
	}

	void import_heterogeneous_medium(scene_context& context, const property_group& properties, std::shared_ptr<medium>& medium)
	{
		auto instance = std::make_shared<heterogeneous_medium>();

		instance->x = 1;
		instance->y = 1;
		instance->z = 1;
		instance->g = 0;

		std::shared_ptr<spectrum> sigma_a = std::make_shared<color_spectrum>(static_cast<real>(0.0011), static_cast<real>(0.0024), static_cast<real>(0.014));
		std::shared_ptr<spectrum> sigma_s = std::make_shared<color_spectrum>(static_cast<real>(2.55), static_cast<real>(3.21), static_cast<real>(3.77));
		
		auto point0 = vector3(0);
		auto point1 = vector3(1);

		std::vector<real> density(1);

		auto scale = std::make_shared<color_spectrum>(static_cast<real>(1));
		
		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "sigma_a") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum(value, sigma_a));
				if (name == "sigma_s") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum(value, sigma_s));
			}

			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = remove_special_character(property.second);

				if (name == "density") META_SCENE_FINISHED_AND_CONTINUE(import_token_real(value, density));
				if (name == "scale") META_SCENE_FINISHED_AND_CONTINUE(scale = std::make_shared<color_spectrum>(string_to_real(value)));
				if (name == "g") META_SCENE_FINISHED_AND_CONTINUE(instance->g = string_to_real(value));
			}

			if (type == PBRT_POINT_TOKEN) {
				const auto value = property.second;

				if (name == "p0") META_SCENE_FINISHED_AND_CONTINUE(import_token_vector3(value, point0));
				if (name == "p1") META_SCENE_FINISHED_AND_CONTINUE(import_token_vector3(value, point1));
			}

			if (type == PBRT_INTEGER_TOKEN) {
				const auto value = string_to_integer<size_t>(remove_special_character(property.second));

				if (name == "nx") META_SCENE_FINISHED_AND_CONTINUE(instance->x = value);
				if (name == "ny") META_SCENE_FINISHED_AND_CONTINUE(instance->y = value);
				if (name == "nz") META_SCENE_FINISHED_AND_CONTINUE(instance->z = value);
			}

			if (type == PBRT_STRING_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "type") continue;
			}

			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		sigma_s = multiply_spectrum(sigma_s, scale);
		sigma_a = multiply_spectrum(sigma_a, scale);

		auto color_sigma_s = std::static_pointer_cast<color_spectrum>(sigma_s);
		auto color_sigma_a = std::static_pointer_cast<color_spectrum>(sigma_a);
		auto sigma_t = (
			color_sigma_s->red + color_sigma_a->red +
			color_sigma_s->green + color_sigma_a->green +
			color_sigma_s->blue + color_sigma_a->blue) / 3;
		
		instance->albedo = std::make_shared<color_spectrum>(color_sigma_s->red / sigma_t, color_sigma_s->green / sigma_t, color_sigma_s->blue / sigma_t);
		instance->sigma_t = std::vector<real>(density.begin(), density.begin() + instance->x * instance->y * instance->z);

		for (size_t index = 0; index < instance->sigma_t.size(); index++)
			instance->sigma_t[index] = instance->sigma_t[index] * sigma_t;

		instance->transform = context.current().transform * (translate<real>(-point0) * math::scale<real>(point1 - point0));
		
		medium = instance;
	}
	
	void import_medium_from_property_group(scene_context& context, const property_group& properties, std::shared_ptr<medium>& medium)
	{
		const auto type = properties.find(type_and_name(PBRT_STRING_TOKEN, "type"))->second;

		std::shared_ptr<metascene::medium> instance = nullptr;

		if (type == "heterogeneous") import_heterogeneous_medium(context, properties, instance);
		if (type == "homogeneous") import_homogeneous_medium(context, properties, instance);
		
		META_SCENE_IMPORT_SUCCESS_CHECK(instance);

		medium = instance;
	}

	void import_named_medium(scene_context& context)
	{
		// the first token should be the name of material
		const auto name = remove_special_character(context.peek_one_token());

		property_group properties;

		context.loop_important_token([&]()
			{
				const auto type_and_name = context.peek_type_and_name();
				const auto value = context.peek_one_token();

				properties.insert({ type_and_name, value });
			});

		properties[type_and_name(PBRT_STRING_TOKEN, "type")] =
			read_string_from_token(properties[type_and_name(PBRT_STRING_TOKEN, "type")]);

		import_medium_from_property_group(context, properties, context.state.media[name]);
	}

	void import_named_medium(scene_context& context, std::shared_ptr<medium>& medium)
	{
		const auto name = read_string_from_token(context.peek_one_token());

		medium = context.state.find_medium(name);
	}

	void import_medium_interface(scene_context& context, std::shared_ptr<metascene::media::media>& media)
	{
		const auto interior = read_string_from_token(context.peek_one_token());
		const auto exterior = read_string_from_token(context.peek_one_token());

		media = std::make_shared<metascene::media::media>();
		
		media->inside = context.state.find_medium(interior);
		media->outside = context.state.find_medium(exterior);
	}
}

#endif