#pragma once

#include "meta-scene/emitters/emitter.hpp"

#include "../emitters/emitter.hpp"

using namespace path_tracing::core::emitters;

namespace path_tracing::core::converter {

	std::shared_ptr<emitter> create_emitter(const std::shared_ptr<metascene::emitters::emitter>& emitter);
	
}
