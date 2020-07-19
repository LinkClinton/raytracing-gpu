#include "import_pbrt_material.hpp"

#include "import_pbrt_texture.hpp"

#include "../../materials/translucent_material.hpp"
#include "../../materials/subsurface_material.hpp"
#include "../../materials/substrate_material.hpp"
#include "../../materials/diffuse_material.hpp"
#include "../../materials/plastic_material.hpp"
#include "../../materials/mixture_material.hpp"
#include "../../materials/mirror_material.hpp"
#include "../../materials/glass_material.hpp"
#include "../../materials/metal_material.hpp"
#include "../../materials/uber_material.hpp"

#include "../../logs.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	constexpr auto META_SCENE_PBRT_BUMP_MAP_IS_NOT_SUPPORT = "pbrt importer : bump map is not supported.";
	
	void import_plastic_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<plastic_material>();

		// initialize pbrt plastic material default value
		instance->specular = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0.25)));
		instance->diffuse = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0.25)));
		instance->roughness = std::make_shared<constant_texture>(static_cast<real>(0.1));
		instance->eta = std::make_shared<constant_texture>(static_cast<real>(1.5));
		
		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->diffuse));
				if (name == "Ks") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->specular));
			}

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "Ks") META_SCENE_FINISHED_AND_CONTINUE(instance->specular = context.state.find_texture(value));
				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(instance->diffuse = context.state.find_texture(value));

				if (name == "roughness") META_SCENE_FINISHED_AND_CONTINUE(instance->roughness = context.state.find_texture(value));
				
				// now we do not support it.
				if (name == "bumpmap") META_SCENE_FINISHED_AND_CONTINUE(logs::warn(META_SCENE_PBRT_BUMP_MAP_IS_NOT_SUPPORT));
			}
			
			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = remove_special_character(property.second);

				if (name == "roughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness));
			}

			// material name
			if (type == PBRT_STRING_TOKEN) continue;

			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		material = instance;
	}

	void import_glass_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		const auto instance = std::make_shared<glass_material>();

		// initialize pbrt glass material default value
		instance->transmission = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(1)));
		instance->reflectance = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(1)));
		instance->roughness_u = std::make_shared<constant_texture>(static_cast<real>(0));
		instance->roughness_v = std::make_shared<constant_texture>(static_cast<real>(0));
		instance->eta = std::make_shared<constant_texture>(static_cast<real>(1.5));
		instance->remapped_roughness_to_alpha = true;
		
		for (const auto& property : properties)
		{
			auto [type, name] = property.first;

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "Kr") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->reflectance));
				if (name == "Kt") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->transmission));
			}

			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = remove_special_character(property.second);

				if (name == "uroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_u));
				if (name == "vroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_v));
				if (name == "index") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->eta));
			}

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);
				
				// now we do not support it.
				if (name == "bumpmap") META_SCENE_FINISHED_AND_CONTINUE(logs::warn(META_SCENE_PBRT_BUMP_MAP_IS_NOT_SUPPORT));
			}

			if (type == PBRT_BOOL_TOKEN) {
				const auto value = string_to_bool(read_string_from_token(property.second));

				if (name == "remaproughness") META_SCENE_FINISHED_AND_CONTINUE(instance->remapped_roughness_to_alpha = value);
			}

			// material name
			if (type == PBRT_STRING_TOKEN) continue;

			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		material = instance;
	}

	void import_metal_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<metal_material>();

		// initialize pbrt metal material default value
		instance->eta = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0)));
		instance->k = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0)));
		instance->roughness_u = std::make_shared<constant_texture>(static_cast<real>(0.01));
		instance->roughness_v = std::make_shared<constant_texture>(static_cast<real>(0.01));
		instance->remapped_roughness_to_alpha = true;
		
		for (const auto& property : properties)
		{
			auto [type, name] = property.first;

			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = remove_special_character(property.second);

				if (name == "uroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_u));
				if (name == "vroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_v));

				if (name == "roughness") {
					import_real_texture(value, instance->roughness_u);
					import_real_texture(value, instance->roughness_v);

					continue;
				}
			}

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "eta") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->eta));
				if (name == "k") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->k));
			}

			if (type == PBRT_SPECTRUM_TOKEN) {
				const auto value = read_string_from_token(property.second);
				
				if (name == "eta") META_SCENE_FINISHED_AND_CONTINUE(import_sampled_spectrum_texture(context.directory_path + value, instance->eta));
				if (name == "k") META_SCENE_FINISHED_AND_CONTINUE(import_sampled_spectrum_texture(context.directory_path + value, instance->k));
			}

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "roughness") {
					instance->roughness_u = context.state.find_texture(value);
					instance->roughness_v = context.state.find_texture(value);

					continue;
				}

				if (name == "bumpmap") META_SCENE_FINISHED_AND_CONTINUE(logs::warn(META_SCENE_PBRT_BUMP_MAP_IS_NOT_SUPPORT));
			}

			if (type == PBRT_BOOL_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "remaproughness") META_SCENE_FINISHED_AND_CONTINUE(instance->remapped_roughness_to_alpha = string_to_bool(value));
			}
			
			// material name
			if (type == PBRT_STRING_TOKEN) continue;

			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		material = instance;
	}

	void import_matte_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<diffuse_material>();

		// initialize pbrt matte material default value
		instance->reflectance = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0.5)));
		instance->sigma = std::make_shared<constant_texture>(static_cast<real>(0));
		
		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->reflectance));
			}

			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = remove_special_character(property.second);

				if (name == "sigma") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->sigma));
			}

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(instance->reflectance = context.state.find_texture(value));
				if (name == "sigma") META_SCENE_FINISHED_AND_CONTINUE(instance->sigma = context.state.find_texture(value));
				if (name == "bumpmap") META_SCENE_FINISHED_AND_CONTINUE(logs::warn(META_SCENE_PBRT_BUMP_MAP_IS_NOT_SUPPORT));
			}

			if (type == PBRT_SPECTRUM_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(import_sampled_spectrum_texture(context.directory_path + value, instance->reflectance));
			}
			
			// material name
			if (type == PBRT_STRING_TOKEN) continue;

			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		material = instance;
	}

	void import_uber_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<uber_material>();

		// initialize pbrt uber material default value
		instance->specular = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0.25)));
		instance->diffuse = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0.25)));

		instance->transmission = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0)));
		instance->reflectance = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0)));

		instance->opacity = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(1)));
		
		instance->roughness_u = std::make_shared<constant_texture>(static_cast<real>(0.1));
		instance->roughness_v = std::make_shared<constant_texture>(static_cast<real>(0.1));
		instance->eta = std::make_shared<constant_texture>(static_cast<real>(1.5));

		instance->remapped_roughness_to_alpha = true;
		
		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = remove_special_character(property.second);

				if (name == "uroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_u));
				if (name == "vroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_v));
				
				if (name == "index") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->eta));

				if (name == "roughness") {
					import_real_texture(value, instance->roughness_u);
					import_real_texture(value, instance->roughness_v);

					continue;
				}
			}

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "Ks") META_SCENE_FINISHED_AND_CONTINUE(instance->specular = context.state.find_texture(value));
				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(instance->diffuse = context.state.find_texture(value));

				if (name == "Kt") META_SCENE_FINISHED_AND_CONTINUE(instance->transmission = context.state.find_texture(value));
				if (name == "Kr") META_SCENE_FINISHED_AND_CONTINUE(instance->reflectance = context.state.find_texture(value));

				if (name == "opacity") META_SCENE_FINISHED_AND_CONTINUE(instance->opacity = context.state.find_texture(value));

				if (name == "roughness") {

					instance->roughness_u = context.state.find_texture(value);
					instance->roughness_v = context.state.find_texture(value);
					
					continue;
				}

				// now we do not support bump map
				if (name == "bumpmap") META_SCENE_FINISHED_AND_CONTINUE(logs::warn(META_SCENE_PBRT_BUMP_MAP_IS_NOT_SUPPORT));
			}

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "Ks") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->specular));
				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->diffuse));

				if (name == "Kt") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->transmission));
				if (name == "Kr") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->reflectance));

				if (name == "opacity") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->opacity));
			}

			// material name
			if (type == PBRT_STRING_TOKEN) continue;
			
			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}
		
		material = instance;
	}

	void import_mirror_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<mirror_material>();

		instance->reflectance = std::make_shared<constant_texture>(static_cast<real>(0.9));
		
		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "Kr") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->reflectance));
			}

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);
				
				if (name == "bumpmap") META_SCENE_FINISHED_AND_CONTINUE(logs::warn(META_SCENE_PBRT_BUMP_MAP_IS_NOT_SUPPORT));
			}
			
			// material name
			if (type == PBRT_STRING_TOKEN) continue;
			
			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		material = instance;
	}

	void import_substrate_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<substrate_material>();

		instance->specular = std::make_shared<constant_texture>(static_cast<real>(0.5));
		instance->diffuse = std::make_shared<constant_texture>(static_cast<real>(0.5));

		instance->roughness_u = std::make_shared<constant_texture>(static_cast<real>(0.1));
		instance->roughness_v = std::make_shared<constant_texture>(static_cast<real>(0.1));

		instance->remapped_roughness_to_alpha = true;

		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "Ks") META_SCENE_FINISHED_AND_CONTINUE(instance->specular = context.state.find_texture(value));
				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(instance->diffuse = context.state.find_texture(value));

				if (name == "uroughness") META_SCENE_FINISHED_AND_CONTINUE(instance->roughness_u = context.state.find_texture(value));
				if (name == "vroughness") META_SCENE_FINISHED_AND_CONTINUE(instance->roughness_v = context.state.find_texture(value));

				if (name == "bumpmap") META_SCENE_FINISHED_AND_CONTINUE(logs::warn(META_SCENE_PBRT_BUMP_MAP_IS_NOT_SUPPORT));
			}

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "Ks") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->specular));
				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->diffuse));
			}

			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = remove_special_character(property.second);

				if (name == "uroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_u));
				if (name == "vroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_v));

				if (name == "roughness") {
					import_real_texture(value, instance->roughness_u);
					import_real_texture(value, instance->roughness_v);

					continue;
				}
			}

			if (type == PBRT_BOOL_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "remaproughness") META_SCENE_FINISHED_AND_CONTINUE(instance->remapped_roughness_to_alpha = string_to_bool(value));
			}
			
			// material name
			if (type == PBRT_STRING_TOKEN) continue;

			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		material = instance;
	}

	void import_fourier_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		logs::warn("pbrt importer : fourier material is not supported. we will create default matte material.");

		auto instance = std::make_shared<diffuse_material>();

		instance->reflectance = std::make_shared<constant_texture>(std::make_shared<color_spectrum>(static_cast<real>(0.)));
		instance->sigma = std::make_shared<constant_texture>(static_cast<real>(0));

		material = instance;
	}

	void import_translucent_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<translucent_material>();

		instance->transmission = std::make_shared<constant_texture>(static_cast<real>(0.5));
		instance->reflectance = std::make_shared<constant_texture>(static_cast<real>(0.5));
		instance->specular = std::make_shared<constant_texture>(static_cast<real>(0.25));
		instance->diffuse = std::make_shared<constant_texture>(static_cast<real>(0.25));
		instance->roughness = std::make_shared<constant_texture>(static_cast<real>(0.1));

		instance->remapped_roughness_to_alpha = true;

		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "transmit") META_SCENE_FINISHED_AND_CONTINUE(instance->transmission = context.state.find_texture(value));
				if (name == "reflect") META_SCENE_FINISHED_AND_CONTINUE(instance->reflectance = context.state.find_texture(value));
				
				if (name == "Ks") META_SCENE_FINISHED_AND_CONTINUE(instance->specular = context.state.find_texture(value));
				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(instance->diffuse = context.state.find_texture(value));

				if (name == "roughness") META_SCENE_FINISHED_AND_CONTINUE(instance->roughness = context.state.find_texture(value));
				
				if (name == "bumpmap") META_SCENE_FINISHED_AND_CONTINUE(logs::warn(META_SCENE_PBRT_BUMP_MAP_IS_NOT_SUPPORT));
			}

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "transmit") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->transmission));
				if (name == "reflect") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->reflectance));
				
				if (name == "Ks") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->specular));
				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->diffuse));
			}

			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = remove_special_character(property.second);

				if (name == "roughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness));
			}

			if (type == PBRT_BOOL_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "remaproughness") META_SCENE_FINISHED_AND_CONTINUE(instance->remapped_roughness_to_alpha = string_to_bool(value));
			}

			// material name
			if (type == PBRT_STRING_TOKEN) continue;

			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		material = instance;
	}

	void import_mix_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<mixture_material>();

		instance->alpha = std::make_shared<constant_texture>(static_cast<real>(0.5));

		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "amount") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->alpha));
			}
			
			if (type == PBRT_STRING_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "namedmaterial1") META_SCENE_FINISHED_AND_CONTINUE(instance->materials[0] = context.state.find_material(value));
				if (name == "namedmaterial2") META_SCENE_FINISHED_AND_CONTINUE(instance->materials[1] = context.state.find_material(value));
				if (name == "type") continue;
			}

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "amount") META_SCENE_FINISHED_AND_CONTINUE(instance->alpha = context.state.find_texture(value));
			}

			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		if (instance->materials[0] == nullptr || instance->materials[1] == nullptr)
			logs::error("pbrt importer : the material in mixture material can not be nullptr.");
		
		material = instance;
	}

	void import_kd_subsurface_material(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<subsurface_material>();

		instance->transmission = std::make_shared<constant_texture>(static_cast<real>(1));
		instance->reflectance = std::make_shared<constant_texture>(static_cast<real>(1));
		instance->diffuse = std::make_shared<constant_texture>(static_cast<real>(0.5));
		instance->mfp = std::make_shared<constant_texture>(static_cast<real>(1));
		instance->roughness_u = std::make_shared<constant_texture>(static_cast<real>(0));
		instance->roughness_v = std::make_shared<constant_texture>(static_cast<real>(0));
		instance->eta = std::make_shared<constant_texture>(static_cast<real>(1.33));
		instance->remapped_roughness_to_alpha = true;
		
		for (const auto& property : properties) {
			auto [type, name] = property.first;

			if (type == PBRT_FLOAT_TOKEN) {
				const auto value = remove_special_character(property.second);

				if (name == "eta") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->eta));
				if (name == "mfp") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->mfp));
				if (name == "uroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_u));
				if (name == "vroughness") META_SCENE_FINISHED_AND_CONTINUE(import_real_texture(value, instance->roughness_v));
			}

			if (type == PBRT_STRING_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "type") continue;
			}

			if (type == PBRT_COLOR_TOKEN || type == PBRT_RGB_TOKEN) {
				const auto value = property.second;

				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->diffuse));
				if (name == "Kr") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->reflectance));
				if (name == "mfp") META_SCENE_FINISHED_AND_CONTINUE(import_color_spectrum_texture(value, instance->mfp));
			}

			if (type == PBRT_TEXTURE_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "Kd") META_SCENE_FINISHED_AND_CONTINUE(instance->diffuse = context.state.find_texture(value));
			}

			if (type == PBRT_BOOL_TOKEN) {
				const auto value = read_string_from_token(property.second);

				if (name == "remaproughness") META_SCENE_FINISHED_AND_CONTINUE(instance->remapped_roughness_to_alpha = string_to_bool(value));
			}
			
			META_SCENE_PBRT_UN_RESOLVE_TOKEN;
		}

		material = instance;
	}
	
	void import_material_from_property_group(scene_context& context, const property_group& properties, std::shared_ptr<material>& material)
	{
		const auto type = properties.find(type_and_name(PBRT_STRING_TOKEN, "type"))->second;

		std::shared_ptr<materials::material> instance = nullptr;

		if (type == "translucent") import_translucent_material(context, properties, instance);
		if (type == "kdsubsurface") import_kd_subsurface_material(context, properties, instance);
		if (type == "substrate") import_substrate_material(context, properties, instance);
		if (type == "plastic") import_plastic_material(context, properties, instance);
		if (type == "fourier") import_fourier_material(context, properties, instance);
		if (type == "mirror") import_mirror_material(context, properties, instance);
		if (type == "glass") import_glass_material(context, properties, instance);
		if (type == "metal") import_metal_material(context, properties, instance);
		if (type == "matte") import_matte_material(context, properties, instance);
		if (type == "uber") import_uber_material(context, properties, instance);
		if (type == "mix") import_mix_material(context, properties, instance);
		if (type == "none") { material = nullptr; return; }
		
		META_SCENE_IMPORT_SUCCESS_CHECK(instance);

		material = instance;
	}
	
	void import_material(scene_context& context, std::shared_ptr<material>& material)
	{
		const auto type = remove_special_character(context.peek_one_token());

		property_group properties;

		properties.insert({ type_and_name(PBRT_STRING_TOKEN, "type"), type });

		context.loop_important_token([&]()
			{
				const auto type_and_name = context.peek_type_and_name();
				const auto value = context.peek_one_token();

				properties.insert({ type_and_name, value });
			});

		if (type == "") { material = nullptr; return; }

		import_material_from_property_group(context, properties, material);
	}

	void import_named_material(scene_context& context)
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
		
		import_material_from_property_group(context, properties, context.state.materials[name]);
	}

	void import_named_material(scene_context& context, std::shared_ptr<material>& material)
	{
		const auto name = read_string_from_token(context.peek_one_token());

		material = context.state.find_material(name);
	}
}

#endif