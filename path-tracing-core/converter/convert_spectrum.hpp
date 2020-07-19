#pragma once

#include "meta-scene/spectrums/spectrum.hpp"

#include "../shared/transform.hpp"

using namespace path_tracing::core::shared;

namespace path_tracing::core::converter {

	vector3 read_spectrum(const std::shared_ptr<metascene::spectrums::spectrum>& spectrum);

}
