#ifndef __INDEPENDENT_SAMPLER_HLSL__
#define __INDEPENDENT_SAMPLER_HLSL__

#include "../utilities/random.hlsl"

struct independent_sampler
{
    pcg32_random rng;

    void initialize(uint2 pixel_location, uint sample_index);

    float next_sample();

    float2 next_sample2d();
};

void independent_sampler::initialize(uint2 pixel_location, uint sample_index)
{
    rng.initialize(pixel_location.y * DispatchRaysDimensions().x + pixel_location.x, sample_index);
}

float independent_sampler::next_sample()
{
    return rng.next_float();
}

float2 independent_sampler::next_sample2d()
{
    return float2(next_sample(), next_sample());
}

#endif
