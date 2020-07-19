#include "import_mitsuba_include.hpp"

#ifdef __MITSUBA_IMPORTER__

metascene::vector3 metascene::importers::mitsuba::string_to_vector3(const std::string& value)
{
	std::stringstream stream(value);

	vector3 ret;

	for (auto index = 0; index < 3; index++) {
		std::string number; std::getline(stream, number, ',');

		ret[index] = string_to_real(number);
	}

	return ret;
}

void metascene::importers::mitsuba::loop_all_children(const tinyxml2::XMLNode* node, const std::function<void(const tinyxml2::XMLNode*)>& function)
{
	auto current = node->FirstChild();

	while (current != nullptr) {
		function(current);

		current = current->NextSibling();
	}
}

void metascene::importers::mitsuba::import_float(const tinyxml2::XMLNode* node, real& value)
{
	// <float name = "float_property" value = "1."/>
	value = string_to_real(node->ToElement()->Attribute("value"));
}

void metascene::importers::mitsuba::import_point(const tinyxml2::XMLNode* node, vector3& point)
{
	/*
	 * <point name="center" x="" y="" z=""/>
	 */
	const auto element = node->ToElement();

	point = vector3(
		string_to_real(element->Attribute("x")),
		string_to_real(element->Attribute("y")),
		string_to_real(element->Attribute("z")));
}

#endif
