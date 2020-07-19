#include "import_mitsuba_transform.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_look_at(const tinyxml2::XMLNode* node, matrix4x4& transform)
	{
		/*
		 * <lookAt/look_at origin="x, y, z" target="x, y, z" up="x, y, z"/>
		 */
		const auto element = node->ToElement();

		const auto origin = string_to_vector3(element->Attribute("origin"));
		const auto target = string_to_vector3(element->Attribute("target"));
		const auto up = string_to_vector3(element->Attribute("up"));

		transform *= inverse(look_at_right_hand<real>(origin, target, up));
	}

	void import_translate(const tinyxml2::XMLNode* node, matrix4x4& transform)
	{
		/*
		 * <translate x="" y="" z=""/>
		 */
		const auto element = node->ToElement();

		const auto x = string_to_real(element->Attribute("x"));
		const auto y = string_to_real(element->Attribute("y"));
		const auto z = string_to_real(element->Attribute("z"));

		transform *= translate<real>(vector3(x, y, z));
	}
	
	void import_transform(const tinyxml2::XMLNode* node, matrix4x4& transform)
	{
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				// "look_at" is mitsuba 2.0 name, "lookAt" is mitsuba 0.6 name
				if (current->Value() == std::string("look_at") || current->Value() == std::string("lookAt"))
					import_look_at(current, transform);

				if (current->Value() == MITSUBA_TRANSLATE_ELEMENT)
					import_translate(current, transform);

				//todo: solve other transform
			});
	}
}

#endif