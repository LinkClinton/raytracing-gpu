#include "import_mitsuba_bsdf.hpp"

#include "../../materials/diffuse_material.hpp"
#include "../../materials/plastic_material.hpp"

#include "import_mitsuba_texture.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_diffuse_bsdf(const tinyxml2::XMLNode* node, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<diffuse_material>();
		
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_SPECTRUM_ELEMENT)
					import_spectrum_texture(current, instance->reflectance);

				if (current->Value() == MITSUBA_RGB_ELEMENT)
					import_rgb_texture(current, instance->reflectance);
			});

		material = instance;
	}

	void import_plastic_diffuse_reflectance(const tinyxml2::XMLNode* node, std::shared_ptr<texture>& diffuse)
	{
		const auto name = std::string(node->ToElement()->Attribute("name"));

		if (name != "diffuse_reflectance" && name != "diffuseReflectance")
			return;

		if (node->Value() == MITSUBA_RGB_ELEMENT)
			import_rgb_texture(node, diffuse);

		if (node->Value() == MITSUBA_SPECTRUM_ELEMENT)
			import_spectrum_texture(node, diffuse);
	}

	void import_plastic_specular_reflectance(const tinyxml2::XMLNode* node, std::shared_ptr<texture>& specular)
	{
		const auto name = std::string(node->ToElement()->Attribute("name"));

		if (name != "specular_reflectance" && name != "specularReflectance")
			return;

		if (node->Value() == MITSUBA_RGB_ELEMENT)
			import_rgb_texture(node, specular);

		if (node->Value() == MITSUBA_SPECTRUM_ELEMENT)
			import_spectrum_texture(node, specular);
	}

	void import_plastic_int_ior(const tinyxml2::XMLNode* node, real& ior)
	{
		const auto name = std::string(node->ToElement()->Attribute("name"));

		if (name != "intIOR" && name != "int_ior") return;

		import_float(node, ior);
	}

	void import_plastic_ext_ior(const tinyxml2::XMLNode* node, real& ior)
	{
		const auto name = std::string(node->ToElement()->Attribute("name"));

		if (name != "extIOR" && name != "ext_ior") return;

		import_float(node, ior);
	}
	
	void import_plastic_alpha(const tinyxml2::XMLNode* node, std::shared_ptr<texture>& roughness)
	{
		const auto name = std::string(node->ToElement()->Attribute("name"));

		if (name != "alpha") return;

		import_float_texture(node, roughness);
	}
	
	void import_plastic_bsdf(const tinyxml2::XMLNode* node, std::shared_ptr<material>& material)
	{
		auto instance = std::make_shared<plastic_material>();

		instance->remapped_roughness_to_alpha = false;
		
		auto int_ior = static_cast<real>(1.5);
		auto ext_ior = static_cast<real>(1);
		
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				import_plastic_specular_reflectance(current, instance->specular);
				import_plastic_diffuse_reflectance(current, instance->diffuse);
				import_plastic_alpha(current, instance->roughness);
				import_plastic_int_ior(current, int_ior);
				import_plastic_ext_ior(current, ext_ior);
			});
		
		instance->eta = std::make_shared<constant_texture>(int_ior / ext_ior);

		material = instance;
	}
	
	void import_bsdf(const tinyxml2::XMLNode* node, const std::shared_ptr<scene_cache>& cache, std::shared_ptr<material>& material)
	{
		const auto element = node->ToElement();
		const auto type = std::string(element->Attribute("type"));
		const auto id = element->Attribute("id");

		if (type == "diffuse") import_diffuse_bsdf(node, material);
		if (type == "roughplastic") import_plastic_bsdf(node, material);

		if (id != nullptr) cache->materials.insert({ std::string(id), material });
	}
}

#endif