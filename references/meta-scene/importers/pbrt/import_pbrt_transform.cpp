#include "import_pbrt_transform.hpp"

#include "import_pbrt_texture.hpp"
#include "import_pbrt_medium.hpp"
#include "import_pbrt_shape.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_coord_sys_transform(scene_context& context, matrix4x4& transform)
	{
		const auto name = read_string_from_token(context.peek_one_token());

		transform = context.state.find_transform(name);
	}

	void import_look_at(scene_context& context, matrix4x4& transform)
	{
		const auto origin = context.peek_vector3();
		const auto target = context.peek_vector3();
		const auto up = context.peek_vector3();

		transform *= look_at_left_hand<real>(origin, target, up);
	}

	void import_translate(scene_context& context, matrix4x4& transform)
	{
		const auto vector3 = context.peek_vector3();

		transform *= translate<real>(vector3);
	}

	void import_rotate(scene_context& context, matrix4x4& transform)
	{
		const auto angle = radians(context.peek_real());
		const auto axis = context.peek_vector3();
			
		transform *= rotate<real>(axis, angle);
	}

	void import_scale(scene_context& context, matrix4x4& transform)
	{
		const auto vector3 = context.peek_vector3();

		transform *= scale<real>(vector3);
	}

	void import_matrix(scene_context& context, matrix4x4& transform)
	{
		const auto data = remove_special_character(context.peek_one_token());

		std::stringstream stream(data);

		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				stream >> transform[x][y];
			}
		}
	}

	void import_concat_matrix(scene_context& context, matrix4x4& transform)
	{
		auto matrix = matrix4x4(1);

		import_matrix(context, matrix);
		
		transform *= matrix;
	}

	void import_transform(scene_context& context)
	{
		context.push_config();

		context.loop_transform_token([&]()
			{
				const auto important_token = context.peek_one_token();

				if (important_token == PBRT_SHAPE_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_shape_to(context));

				if (important_token == PBRT_TRANSFORM_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_matrix(context, context.current().transform));

				if (important_token == PBRT_SCALE_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_scale(context, context.current().transform));

				if (important_token == PBRT_ROTATE_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_rotate(context, context.current().transform));
			
				if (important_token == PBRT_TRANSLATE_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_translate(context, context.current().transform));
			
				if (important_token == PBRT_OBJECT_INSTANCE_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_objects_to(context));

				if (important_token == PBRT_MAKE_TEXTURE_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_texture(context));

				if (important_token == PBRT_MAKE_NAMED_MEDIUM_TOKEN) META_SCENE_FINISHED_AND_RETURN(import_named_medium(context));
			
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});
		
		context.peek_one_token();
		context.pop_config();
	}

}

#endif