#include "import_pbrt_attribute.hpp"

#include "import_pbrt_light_source.hpp"
#include "import_pbrt_transform.hpp"
#include "import_pbrt_material.hpp"
#include "import_pbrt_texture.hpp"
#include "import_pbrt_medium.hpp"
#include "import_pbrt_shape.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_attribute(scene_context& context)
	{
		context.push_config();
		
		context.loop_attribute_token([&]()
			{
				const auto important_token = context.peek_one_token();

				if (important_token == PBRT_REVERSE_ORIENTATION_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_reverse_orientation(context));

				if (important_token == PBRT_COORD_SYS_TRANSFORM_TOKEN)
					META_SCENE_FINISHED_AND_RETURN(import_coord_sys_transform(context, context.current().transform));
			
				if (important_token == PBRT_TRANSLATE_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_translate(context, context.current().transform));
			
				if (important_token == PBRT_ROTATE_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_rotate(context, context.current().transform));

				if (important_token == PBRT_SCALE_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_scale(context, context.current().transform));

				if (important_token == PBRT_TRANSFORM_TOKEN)
					META_SCENE_FINISHED_AND_RETURN(import_matrix(context, context.current().transform));
			
				if (important_token == PBRT_CONCAT_TRANSFORM_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_concat_matrix(context, context.current().transform));
			
				if (important_token == PBRT_AREA_LIGHT_SOURCE_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_area_light_source(context, context.current().emitter));

				if (important_token == PBRT_LIGHT_SOURCE_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_light_source(context));

				if (important_token == PBRT_MATERIAL_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_material(context, context.current().material));

				if (important_token == PBRT_SHAPE_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_shape_to(context));

				if (important_token == PBRT_NAMED_MATERIAL_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_named_material(context, context.current().material));

				if (important_token == PBRT_MEDIUM_INTERFACE_TOKEN)
					META_SCENE_FINISHED_AND_RETURN(import_medium_interface(context, context.current().media));
			
				if (important_token == PBRT_ATTRIBUTE_BEGIN_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_attribute(context));
			
				if (important_token == PBRT_OBJECT_BEGIN_TOKEN) 
					META_SCENE_FINISHED_AND_RETURN(import_objects(context));

				if (important_token == PBRT_OBJECT_INSTANCE_TOKEN)
					META_SCENE_FINISHED_AND_RETURN(import_objects_to(context));

				if (important_token == PBRT_MAKE_TEXTURE_TOKEN)
					META_SCENE_FINISHED_AND_RETURN(import_texture(context));

				if (important_token == PBRT_MAKE_NAMED_MATERIAL_TOKEN)
					META_SCENE_FINISHED_AND_RETURN(import_named_material(context));

				if (important_token == PBRT_MAKE_NAMED_MEDIUM_TOKEN)
					META_SCENE_FINISHED_AND_RETURN(import_named_medium(context));

				if (important_token == PBRT_TRANSFORM_BEGIN_TOKEN)
					META_SCENE_FINISHED_AND_RETURN(import_transform(context));
			
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		// the last token should be PBRT_ATTRIBUTE_END_TOKEN
		context.peek_one_token();
		context.pop_config();
	}
}

#endif