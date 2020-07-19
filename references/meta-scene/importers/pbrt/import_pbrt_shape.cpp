#include "import_pbrt_shape.hpp"

#include "../../shapes/triangles.hpp"
#include "../../shapes/sphere.hpp"
#include "../../shapes/curve.hpp"
#include "../../shapes/mesh.hpp"
#include "../../shapes/disk.hpp"
#include "../../logs.hpp"

#include "import_pbrt_attribute.hpp"
#include "import_pbrt_material.hpp"
#include "import_pbrt_texture.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	constexpr auto META_SCENE_PBRT_ALPHA_IS_NOT_SUPPORT = "pbrt importer : alpha texture is not support.";
	constexpr auto META_SCENE_PBRT_SHADOW_ALPHA_IS_NOT_SUPPORT = "pbrt importer : shadow alpha texture is not support.";

	
	void import_triangle_mesh(scene_context& context, std::shared_ptr<shape>& shape)
	{
		auto instance = std::make_shared<triangles>();

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_POINT_TOKEN) {
					const auto token = context.peek_one_token();

					if (name == "P") META_SCENE_FINISHED_AND_RETURN(import_token_vector3(token, instance->positions));
				}

				if (type == PBRT_FLOAT_TOKEN) {
					const auto token = context.peek_one_token();

					if (name == "uv") META_SCENE_FINISHED_AND_RETURN(import_token_vector2(token, instance->uvs));
					if (name == "st") META_SCENE_FINISHED_AND_RETURN(import_token_vector2(token, instance->uvs));
				}

				if (type == PBRT_INTEGER_TOKEN) {
					const auto token = context.peek_one_token();

					if (name == "indices") META_SCENE_FINISHED_AND_RETURN(import_token_unsigned(token, instance->indices));
					if (name == "nlevels") return;
				}

				if (type == PBRT_NORMAL_TOKEN) {
					const auto token = context.peek_one_token();

					if (name == "N") META_SCENE_FINISHED_AND_RETURN(import_token_vector3(token, instance->normals));
				}
			
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		shape = instance;
	}

	void import_ply_mesh(scene_context& context, std::shared_ptr<shape>& shape)
	{
		auto instance = std::make_shared<mesh>();

		instance->mesh_type = mesh_type::ply;
		instance->filename = "";
		instance->mask = nullptr;
		
		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_STRING_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "filename") META_SCENE_FINISHED_AND_RETURN(instance->filename = context.directory_path + value);
				}

				if (type == PBRT_TEXTURE_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "alpha") META_SCENE_FINISHED_AND_RETURN(instance->mask = context.state.find_texture(value));
					if (name == "shadowalpha") META_SCENE_FINISHED_AND_RETURN(instance->mask = context.state.find_texture(value));
				}

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = remove_special_character(context.peek_one_token());

					if (name == "alpha") META_SCENE_FINISHED_AND_RETURN(import_real_texture(value, instance->mask));
					if (name == "shadowalpha") META_SCENE_FINISHED_AND_RETURN(import_real_texture(value, instance->mask));
				}
			
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		shape = instance;
	}

	void import_obj_mesh(scene_context& context, std::shared_ptr<shape>& shape)
	{
		auto instance = std::make_shared<mesh>();

		instance->mesh_type = mesh_type::obj;
		instance->filename = "";
		instance->mask = nullptr;

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_STRING_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "filename") META_SCENE_FINISHED_AND_RETURN(instance->filename = context.directory_path + value);
				}

				if (type == PBRT_TEXTURE_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "alpha") META_SCENE_FINISHED_AND_RETURN(instance->mask = context.state.find_texture(value));
					if (name == "shadowalpha") META_SCENE_FINISHED_AND_RETURN(instance->mask = context.state.find_texture(value));
				}

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = remove_special_character(context.peek_one_token());

					if (name == "alpha") META_SCENE_FINISHED_AND_RETURN(import_real_texture(value, instance->mask));
					if (name == "shadowalpha") META_SCENE_FINISHED_AND_RETURN(import_real_texture(value, instance->mask));
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		shape = instance;
	}

	void import_sphere(scene_context& context, std::shared_ptr<shape>& shape)
	{
		auto instance = std::make_shared<sphere>();

		instance->radius = static_cast<real>(1);

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = string_to_real(remove_special_character(context.peek_one_token()));

					if (name == "radius") META_SCENE_FINISHED_AND_RETURN(instance->radius = value);
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		shape = instance;
	}

	void import_disk(scene_context& context, std::shared_ptr<shape>& shape)
	{
		auto instance = std::make_shared<disk>();

		instance->radius = 1;
		instance->height = 0;

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "radius") META_SCENE_FINISHED_AND_RETURN(instance->radius = value);
					if (name == "height") META_SCENE_FINISHED_AND_RETURN(instance->height = value);
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		shape = instance;
	}

	void import_curve(scene_context& context, std::shared_ptr<shape>& shape)
	{
		auto instance = std::make_shared<curve>();

		instance->control_points = {};
		instance->width = { 1, 1 };
		instance->u_min = 0;
		instance->u_max = 1;

		std::vector<vector3> control_points;
		
		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_STRING_TOKEN) {
					const auto value = read_string_from_token(context.peek_one_token());

					if (name == "type" && value == "cylinder") return;
				}

				if (type == PBRT_POINT_TOKEN) {
					const auto value = context.peek_one_token();

					if (name == "P") META_SCENE_FINISHED_AND_RETURN(import_token_vector3(value, control_points));
				}

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "width0") META_SCENE_FINISHED_AND_RETURN(instance->width[0] = value);
					if (name == "width1") META_SCENE_FINISHED_AND_RETURN(instance->width[1] = value);
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		instance->control_points = { control_points[0], control_points[1], control_points[2], control_points[3] };
		
		shape = instance;
	}

	void import_shape(scene_context& context, std::shared_ptr<entity>& entity)
	{
		const auto type = remove_special_character(context.peek_one_token());

		entity = std::make_shared<metascene::entity>();

		if (type == "loopsubdiv") import_triangle_mesh(context, entity->shape);
		if (type == "trianglemesh") import_triangle_mesh(context, entity->shape);
		if (type == "plymesh") import_ply_mesh(context, entity->shape);
		if (type == "objmesh") import_obj_mesh(context, entity->shape);
		if (type == "sphere") import_sphere(context, entity->shape);
		if (type == "curve") import_curve(context, entity->shape);
		if (type == "disk") import_disk(context, entity->shape);
		
		// build the shape we current state of context
		entity->shape->reverse_orientation = context.current().reverse_orientation;
		entity->transform = context.current().transform;
		entity->material = context.current().material;
		entity->emitter = context.current().emitter;
		entity->media = context.current().media;

		META_SCENE_IMPORT_SUCCESS_CHECK(entity->shape);
	}

	void import_shape_to(scene_context& context)
	{
		std::shared_ptr<entity> entity;

		import_shape(context, entity);

		if (context.current().objects != nullptr)
			context.current().objects->entities.push_back(entity);
		else
			context.scene->entities.push_back(entity);
		
		META_SCENE_IMPORT_SUCCESS_CHECK(entity->shape);
	}

	void import_objects_to(scene_context& context)
	{
		const auto name = read_string_from_token(context.peek_one_token());

		const auto objects = context.state.find_object(name);

		for (const auto& object : objects->entities) {
			auto entity = std::make_shared<metascene::entity>();

			entity->material = object->material;
			entity->emitter = object->emitter;
			entity->shape = object->shape;
			entity->media = object->media;

			entity->transform = context.current().transform * object->transform;

			if (context.current().objects != nullptr)
				context.current().objects->entities.push_back(entity);
			else
				context.scene->entities.push_back(entity);
		}
	}

	void import_objects(scene_context& context)
	{
		const auto name = remove_special_character(context.peek_one_token());
		const auto objects = std::make_shared<pbrt::objects>();
		
		context.push_config();		
		context.current().objects = objects;
		
		context.loop_objects_token([&]()
			{
				const auto important_token = context.peek_one_token();

				if (important_token == PBRT_SHAPE_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_shape_to(context));

				if (important_token == PBRT_ATTRIBUTE_BEGIN_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_attribute(context));

				if (important_token == PBRT_MATERIAL_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_material(context, context.current().material));
			
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		context.state.objects.insert({ name, objects });
		
		context.peek_one_token();
		context.pop_config();
	}
}

#endif