#pragma once

#include "entity.hpp"

#include <vector>

namespace path_tracing::core::scenes {

	class scene final : public interfaces::noncopyable {
	public:
		scene() = default;

		~scene() = default;

		void add_entity(const std::shared_ptr<entity>& entity);

		const std::vector<std::shared_ptr<entity>>& entities() const noexcept;

		const std::vector<std::shared_ptr<entity>>& emitters() const noexcept;
	private:
		std::vector<std::shared_ptr<entity>> mEntities;
		std::vector<std::shared_ptr<entity>> mEmitters;
	};
	
}
