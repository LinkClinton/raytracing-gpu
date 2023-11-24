#pragma once

#include "../../types.hpp"

#include <nlohmann/json.hpp>

namespace nlohmann
{
	using matrix4x4 = raytracing::matrix4x4;

	using vector4 = raytracing::vector4;
	using vector3 = raytracing::vector3;
	using vector2 = raytracing::vector2;

	template <>
	struct adl_serializer<matrix4x4>
	{
		static void to_json(json& json, const matrix4x4& matrix)
		{
			json = 
			{
				matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
				matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
				matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
				matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3],
			};
		}

		static void from_json(const json& json, matrix4x4& matrix)
		{
			matrix[0][0] = json[0];  matrix[0][1] = json[1];  matrix[0][2] = json[2];  matrix[0][3] = json[3];
			matrix[1][0] = json[4];  matrix[1][1] = json[5];  matrix[1][2] = json[6];  matrix[1][3] = json[7];
			matrix[2][0] = json[8];  matrix[2][1] = json[9];  matrix[2][2] = json[10]; matrix[2][3] = json[11];
			matrix[3][0] = json[12]; matrix[3][1] = json[13]; matrix[3][2] = json[14]; matrix[3][3] = json[15];
		}
	};

	template <>
	struct adl_serializer<vector3>
	{
		static void to_json(json& json, const vector3& vector)
		{
			json = { vector.x, vector.y, vector.z };
		}

		static void from_json(const json& json, vector3& vector)
		{
			vector = vector3(json[0], json[1], json[2]);
		}
	};

	template <>
	struct adl_serializer<vector2>
	{
		static void to_json(json& json, const vector2& vector)
		{
			json = { vector.x, vector.y };
		}

		static void from_json(const json& json, vector2& vector)
		{
			vector = vector2(json[0], json[1]);
		}
	};


	template <>
	struct adl_serializer<std::vector<vector3>>
	{
		static void to_json(json& json, const std::vector<vector3>& vector)
		{
			for (size_t index = 0; index < vector.size(); index++)
			{
				json[index] = vector[index];
			}
		}

		static void from_json(const json& json, std::vector<vector3>& vector)
		{
			vector.resize(json.size());

			for (size_t index = 0; index < json.size(); index++)
			{
				vector[index] = json[index];
			}
		}
	};

	template <>
	struct adl_serializer<std::vector<vector2>>
	{
		static void to_json(json& json, const std::vector<vector2>& vector)
		{
			for (size_t index = 0; index < vector.size(); index++)
			{
				json[index] = vector[index];
			}
		}

		static void from_json(const json& json, std::vector<vector2>& vector)
		{
			vector.resize(json.size());

			for (size_t index = 0; index < json.size(); index++)
			{
				vector[index] = json[index];
			}
		}
	};

	template <>
	struct adl_serializer<raytracing::scenes::texture>
	{
		static void to_json(json& json, const raytracing::scenes::texture& texture)
		{
			json["image"] = texture.image;
			json["value"] = texture.value;
		}

		static void from_json(const json& json, raytracing::scenes::texture& texture)
		{
			texture.image = json["image"];
			texture.value = json["value"];
		}
	};

}