#pragma once

#include "../interfaces/string_property.hpp"
#include "../utilities.hpp"

namespace metascene {

	namespace media {

		enum class type : uint32 {
			unknown = 0, homogeneous = 1, heterogeneous = 2
		};
		
		struct medium : interfaces::string_property {
			type type = type::unknown;

			medium() = default;

			medium(const media::type& type);
		};

		struct media final : interfaces::string_property {
			std::shared_ptr<medium> outside;
			std::shared_ptr<medium> inside;

			media() = default;

			std::string to_string() const noexcept override;
		};
	}
}
