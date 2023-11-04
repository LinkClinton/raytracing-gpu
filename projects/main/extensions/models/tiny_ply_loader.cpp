#include "tiny_ply_loader.hpp"

#define TINYPLY_IMPLEMENTATION

#include <tinyply/tinyply.h>
#include <fstream>

namespace raytracing::extensions::models
{
	size_t size_of(Type type)
	{
		static mapping<Type, size_t> TypeSizeMapping = 
		{
			{ Type::INT8,  1 },
			{ Type::INT16, 2 },
			{ Type::INT32, 4 },
			{ Type::UINT8, 1 },
			{ Type::UINT16, 2 },
			{ Type::UINT32, 4 },
			{ Type::FLOAT32, 4 },
			{ Type::FLOAT64, 8 }
		};

		return TypeSizeMapping.at(type);
	}

}
