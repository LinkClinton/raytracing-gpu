#include "import_pbrt_texture.hpp"

#include "import_pbrt_spectrum.hpp"

#include "../../textures/constant_texture.hpp"
#include "../../textures/mixture_texture.hpp"
#include "../../textures/image_texture.hpp"
#include "../../textures/scale_texture.hpp"
#include "../../textures/texture.hpp"

#include "../../logs.hpp"

#include <filesystem>

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_color_spectrum_texture(const std::string& token, std::shared_ptr<texture>& texture)
	{
		auto instance = std::make_shared<constant_texture>(value_type::spectrum);

		import_color_spectrum(token, instance->spectrum);

		texture = instance;
	}

	void import_sampled_spectrum_texture(const std::string& filename, std::shared_ptr<texture>& texture)
	{
		auto instance = std::make_shared<constant_texture>(value_type::spectrum);

		import_sampled_spectrum(filename, instance->spectrum);

		texture = instance;
	}

	void import_real_texture(const std::string& token, std::shared_ptr<texture>& texture)
	{
		const auto instance = std::make_shared<constant_texture>(value_type::real);

		instance->real = string_to_real(token);

		texture = instance;
	}

	void import_image_map_texture(scene_context& context, std::shared_ptr<texture>& texture)
	{
		auto instance = std::make_shared<image_texture>();
		
		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_STRING_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "filename") META_SCENE_FINISHED_AND_RETURN(instance->filename = context.directory_path + value);
					if (name == "mapping") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : mapping is not support."));
					if (name == "wrap") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : wrap is not support."));

				}

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "scale") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : scale image map value is not support."))
					
					if (name == "uscale") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : uscale is not support."));
					if (name == "vscale") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : uscale is not support."));

					if (name == "udelta") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : udelta is not support."));
					if (name == "vdelta") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : udelta is not support."));

					if (name == "maxanisotropy") META_SCENE_FINISHED_AND_RETURN(logs::warn("pbrt importer : maxanisotropy is not support"));
				}

				if (type == PBRT_BOOL_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "gamma") META_SCENE_FINISHED_AND_RETURN(instance->gamma = string_to_bool(value));
				}
			
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		const auto extension = std::filesystem::path(instance->filename).extension().string();

		if (extension != ".hdr" && extension != ".exr") instance->gamma = true;
		
		texture = instance;
	}

	void import_scale_texture(scene_context& context, std::shared_ptr<texture>& texture)
	{
		auto instance = std::make_shared<scale_texture>();

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_TEXTURE_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "tex1") META_SCENE_FINISHED_AND_RETURN(instance->base = context.state.find_texture(value));
					if (name == "tex2") META_SCENE_FINISHED_AND_RETURN(instance->scale = context.state.find_texture(value));
				}

				if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
					const auto value = context.peek_one_token();

					if (name == "tex1") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum_texture(value, instance->base));
					if (name == "tex2") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum_texture(value, instance->scale));
				}

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "tex2") META_SCENE_FINISHED_AND_RETURN(instance->scale = std::make_shared<constant_texture>(value));
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		if (instance->scale->type != textures::type::constant)
			META_SCENE_PBRT_SCALE_TEXTURE_SHOULD_CONSTANT;
		
		texture = instance;
	}

	void import_constant_texture(scene_context& context, std::shared_ptr<texture>& texture)
	{
		auto instance = std::make_shared<constant_texture>();

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "value") META_SCENE_FINISHED_AND_RETURN({ instance->value_type = value_type::real; instance->real = value; });
				}

				if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
					const auto value = context.peek_one_token();

					if (name == "value") META_SCENE_FINISHED_AND_RETURN({
						instance->value_type = value_type::spectrum; import_color_spectrum(value, instance->spectrum);
						});
				}
			
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		texture = instance;
	}

	void import_marble_texture(scene_context& context, std::shared_ptr<texture>& texture)
	{
		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "scale") return;
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});
		
		logs::warn("pbrt importer : marble texture is not supported. we will create default constant texture.");

		texture = std::make_shared<constant_texture>(static_cast<real>(1));
	}

	void import_fbm_texture(scene_context& context, std::shared_ptr<texture>& texture)
	{
		context.loop_important_token([&]()
			{
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		logs::warn("pbrt importer : fbm texture is not supported. we will create default constant texture.");

		texture = std::make_shared<constant_texture>(static_cast<real>(1));
	}

	void import_wrinkled_texture(scene_context& context, std::shared_ptr<texture>& texture)
	{
		context.loop_important_token([&]()
			{
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		logs::warn("pbrt importer : wrinkled texture is not supported. we will create default constant texture.");

		texture = std::make_shared<constant_texture>(static_cast<real>(1));
	}

	void import_mixture_texture(scene_context& context, std::shared_ptr<texture>& texture)
	{
		auto instance = std::make_shared<mixture_texture>();

		instance->texture0 = std::make_shared<constant_texture>(static_cast<real>(0));
		instance->texture1 = std::make_shared<constant_texture>(static_cast<real>(1));
		instance->alpha = std::make_shared<constant_texture>(static_cast<real>(0.5));

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();
			
				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "tex1") META_SCENE_FINISHED_AND_RETURN(instance->texture0 = std::make_shared<constant_texture>(static_cast<real>(value)));
					if (name == "tex2") META_SCENE_FINISHED_AND_RETURN(instance->texture1 = std::make_shared<constant_texture>(static_cast<real>(value)));
					if (name == "amount") META_SCENE_FINISHED_AND_RETURN(instance->alpha = std::make_shared<constant_texture>(static_cast<real>(value)));
				}

				if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
					const auto value = context.peek_one_token();

					if (name == "tex1") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum_texture(value, instance->texture0));
					if (name == "tex2") META_SCENE_FINISHED_AND_RETURN(import_color_spectrum_texture(value, instance->texture1));
				}

				if (type == PBRT_TEXTURE_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "tex1") META_SCENE_FINISHED_AND_RETURN(instance->texture0 = context.state.find_texture(value));
					if (name == "tex2") META_SCENE_FINISHED_AND_RETURN(instance->texture1 = context.state.find_texture(value));
					if (name == "amount") META_SCENE_FINISHED_AND_RETURN(instance->alpha = context.state.find_texture(value));
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		texture = instance;
	}

	void import_windy_texture(scene_context& context, std::shared_ptr<texture>& texture)
	{
		context.loop_important_token([&]()
			{
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		logs::warn("pbrt importer : windy texture is not supported. we will create default constant texture.");

		texture = std::make_shared<constant_texture>(static_cast<real>(1));
	}

	void import_texture(scene_context& context)
	{
		const auto name = remove_special_character(context.peek_one_token());
		const auto color = remove_special_character(context.peek_one_token());
		const auto type = remove_special_character(context.peek_one_token());

		std::shared_ptr<texture> instance = nullptr;
		
		if (type == "constant") import_constant_texture(context, instance);
		if (type == "wrinkled") import_wrinkled_texture(context, instance);
		if (type == "imagemap") import_image_map_texture(context, instance);
		if (type == "marble") import_marble_texture(context, instance);
		if (type == "scale") import_scale_texture(context, instance);
		if (type == "windy") import_windy_texture(context, instance);
		if (type == "mix") import_mixture_texture(context, instance);
		if (type == "fbm") import_fbm_texture(context, instance);
		
		META_SCENE_IMPORT_SUCCESS_CHECK(instance);

		context.state.textures[name] = instance;
	}

}

#endif