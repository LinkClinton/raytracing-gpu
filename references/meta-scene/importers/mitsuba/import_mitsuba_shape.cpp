#include "import_mitsuba_shape.hpp"

#include "../../shapes/sphere.hpp"
#include "../../shapes/mesh.hpp"

#include "import_mitsuba_transform.hpp"
#include "import_mitsuba_emitter.hpp"
#include "import_mitsuba_bsdf.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_reference_in_shape(const tinyxml2::XMLNode* node, const std::shared_ptr<scene_cache>& cache, const std::shared_ptr<entity>& entity)
	{
		entity->material = cache->materials.at(node->ToElement()->Attribute("id"));
	}
	
	void import_obj_mesh(const tinyxml2::XMLNode* node, const std::shared_ptr<scene_cache>& cache, std::shared_ptr<shape>& shape)
	{
		// <string name = "filename" value = "path"/>
		const auto path = node->ToElement()->Attribute("value");

		shape = std::make_shared<mesh>(mesh_type::obj, cache->directory_path + "/" + path);
	}

	void import_sphere_center(const tinyxml2::XMLNode* node, vector3& center)
	{
		if (node->ToElement()->Attribute("name") != std::string("center"))
			return;
		
		import_point(node, center);
	}

	void import_sphere_radius(const tinyxml2::XMLNode* node, real& radius)
	{
		if (node->ToElement()->Attribute("name") != std::string("radius"))
			return;

		import_float(node, radius);
	}

	void import_sphere(const tinyxml2::XMLNode* node, const std::shared_ptr<scene_cache>& cache)
	{
		auto entity = std::make_shared<metascene::entity>();
		auto sphere = std::make_shared<shapes::sphere>();
		auto center = vector3(0);
		
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_POINT_ELEMENT)
					import_sphere_center(current, center);

				if (current->Value() == MITSUBA_FLOAT_ELEMENT)
					import_sphere_radius(current, sphere->radius);
			
				if (current->Value() == MITSUBA_TRANSFORM_ELEMENT)
					import_transform(current, entity->transform);

				if (current->Value() == MITSUBA_REFERENCE_ELEMENT)
					import_reference_in_shape(current, cache, entity);

				if (current->Value() == MITSUBA_BSDF_ELEMENT)
					import_bsdf(current, cache, entity->material);
			
				if (current->Value() == MITSUBA_EMITTER_ELEMENT)
					import_emitter(current, entity->emitter);
			});

		entity->shape = sphere;
		entity->transform *= translate<real>(center);
		
		cache->scene->entities.push_back(entity);
	}

	void import_obj(const tinyxml2::XMLNode* node, const std::shared_ptr<scene_cache>& cache)
	{
		auto entity = std::make_shared<metascene::entity>();
		
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_STRING_ELEMENT)
					import_obj_mesh(current, cache, entity->shape);

				if (current->Value() == MITSUBA_TRANSFORM_ELEMENT)
					import_transform(current, entity->transform);

				if (current->Value() == MITSUBA_REFERENCE_ELEMENT)
					import_reference_in_shape(current, cache, entity);

				if (current->Value() == MITSUBA_BSDF_ELEMENT)
					import_bsdf(current, cache, entity->material);
			
				if (current->Value() == MITSUBA_EMITTER_ELEMENT)
					import_emitter(current, entity->emitter);
			});

		cache->scene->entities.push_back(entity);
	}

	void import_shape(const tinyxml2::XMLNode* node, const std::shared_ptr<scene_cache>& cache)
	{
		const auto type = std::string(node->ToElement()->Attribute("type"));

		if (type == "sphere") import_sphere(node, cache);
		if (type == "obj") import_obj(node, cache);
	}
}

#endif