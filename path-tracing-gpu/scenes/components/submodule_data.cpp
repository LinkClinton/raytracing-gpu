#include "submodule_data.hpp"

size_t path_tracing::scenes::components::size_of_submodule_data_type(const std::string& type)
{
	static mapping<std::string, size_t> type_size = {
		{ "float3", 12}, { "real", 4 }, { "uint", 4 }, { "texture", 16 }
	};

	return type_size.at(type);
}
