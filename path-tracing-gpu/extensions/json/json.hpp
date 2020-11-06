#pragma once

#include "../../cores/types.hpp"

#include <nlohmann/json.hpp>

namespace nlohmann {

	template <>
	struct adl_serializer<vector4> {
		static void to_json(json& json, const vector4& vector)
		{
			json = { vector.x, vector.y, vector.z, vector.w };
		}

		static void from_json(const json& json, vector4& vector)
		{
			vector = vector4(json[0], json[1], json[2], json[3]);
		}
	};

	template <>
	struct adl_serializer<vector3> {
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
	struct adl_serializer<std::vector<vector3>> {
		static void to_json(json& json, const std::vector<vector3>& vector)
		{
			std::vector<float> floats(vector.size() * 3);

			std::memcpy(floats.data(), vector.data(), sizeof(float) * floats.size());

			json = floats;
		}

		static void from_json(const json& json, std::vector<vector3>& vector)
		{
			std::vector<float> floats = json;

			vector = std::vector<vector3>(floats.size() / 3);

			std::memcpy(vector.data(), floats.data(), sizeof(vector3) * vector.size());
		}
	};

	template <>
	struct adl_serializer<std::vector<vector2>> {
		static void to_json(json& json, const std::vector<vector2>& vector)
		{
			std::vector<float> floats(vector.size() * 2);

			std::memcpy(floats.data(), vector.data(), sizeof(float) * floats.size());

			json = floats;
		}

		static void from_json(const json& json, std::vector<vector2>& vector)
		{
			std::vector<float> floats = json;

			vector = std::vector<vector2>(floats.size() / 2);

			std::memcpy(vector.data(), floats.data(), sizeof(vector2) * vector.size());
		}
	};
	
}