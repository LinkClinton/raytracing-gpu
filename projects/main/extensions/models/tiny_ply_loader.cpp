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

	template <typename TargetType>
	TargetType read(Type type, void* data);

	template <>
	uint32 read(Type type, void* data)
	{
		switch (type)
		{
			case Type::INT8: return static_cast<uint32>(*static_cast<int8*>(data));
			case Type::INT16: return static_cast<uint32>(*static_cast<int16*>(data));
			case Type::INT32: return static_cast<uint32>(*static_cast<int32*>(data));
			case Type::UINT8: return static_cast<uint32>(*static_cast<uint8*>(data));
			case Type::UINT16: return static_cast<uint32>(*static_cast<uint16*>(data));
			case Type::UINT32: return static_cast<uint32>(*static_cast<uint32*>(data));
			default: throw "invalid";
		}
	}

	template <>
	real read(Type type, void* data)
	{
		switch (type)
		{
			case Type::FLOAT32: return static_cast<real>(*static_cast<float*>(data));
			case Type::FLOAT64: return static_cast<real>(*static_cast<double*>(data));
			default: throw "invalid";
		}
	}

	void read_uint32_array(const std::shared_ptr<PlyData>& data, std::vector<uint32>& output)
	{
		if (data == nullptr)
		{
			return;
		}

		const size_t offset = size_of(data->t);

		for (size_t index = 0; index < data->buffer.size_bytes(); index += offset)
		{
			output.push_back(read<uint32>(data->t, data->buffer.get() + index));
		}
	}

	void read_vector3_array(const std::shared_ptr<PlyData>& data, std::vector<vector3>& output)
	{
		if (data == nullptr)
		{
			return;
		}

		const size_t offset = size_of(data->t) * 3;

		for (size_t index = 0; index < data->buffer.size_bytes(); index += offset)
		{
			output.emplace_back(
				read<real>(data->t, data->buffer.get() + index + sizeof(real) * 0),
				read<real>(data->t, data->buffer.get() + index + sizeof(real) * 1),
				read<real>(data->t, data->buffer.get() + index + sizeof(real) * 2)
			);
		}
	}

	void read_vector2_array(const std::shared_ptr<PlyData>& data, std::vector<vector3>& output)
	{
		if (data == nullptr)
		{
			return;
		}

		const size_t offset = size_of(data->t) * 2;

		for (size_t index = 0; index < data->buffer.size_bytes(); index += offset)
		{
			output.emplace_back(
				read<real>(data->t, data->buffer.get() + index + sizeof(real) * 0),
				read<real>(data->t, data->buffer.get() + index + sizeof(real) * 1),
				0
			);
		}
	}

	runtime::resources::components::mesh load_ply_mesh(const std::string& filename)
	{
		auto stream = std::ifstream(filename, std::ios::binary);

		PlyFile file;

		file.parse_header(stream);

		std::shared_ptr<PlyData> positions = file.request_properties_from_element("vertex", { "x", "y", "z" });
		std::shared_ptr<PlyData> normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" });
		std::shared_ptr<PlyData> uvs = file.request_properties_from_element("vertex", { "u", "v" });
		std::shared_ptr<PlyData> indices = file.request_properties_from_element("face", { "vertex_indices" });

		file.read(stream);

		runtime::resources::components::mesh mesh;

		read_vector3_array(positions, mesh.data.positions);
		read_vector3_array(normals, mesh.data.normals);
		read_vector2_array(uvs, mesh.data.uvs);
		read_uint32_array(indices, mesh.data.indices);

		mesh.info.vtx_count = static_cast<uint32>(mesh.data.positions.size());
		mesh.info.idx_count = static_cast<uint32>(mesh.data.indices.size());

		assert(mesh.info.vtx_count != 0);
		assert(mesh.info.idx_count != 0);

		mesh.info.attribute |= runtime::resources::components::mesh_attribute::position;
		mesh.info.attribute |= runtime::resources::components::mesh_attribute::index;

		if (!mesh.data.normals.empty())
		{
			mesh.info.attribute |= runtime::resources::components::mesh_attribute::normal;
		}

		if (!mesh.data.uvs.empty())
		{
			mesh.info.attribute |= runtime::resources::components::mesh_attribute::uv;
		}

		return mesh;
	}
}
