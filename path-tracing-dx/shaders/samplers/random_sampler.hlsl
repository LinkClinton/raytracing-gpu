#include "random_generator.hlsl"

#ifndef __RANDOM_SAMPLER_HLSL__
#define __RANDOM_SAMPLER_HLSL__

struct random_sampler {
	random_generator generator;
};

random_sampler create_random_sampler(uint sample_index) 
{
	random_sampler sampler;

	sampler.generator = create_random_generator(sample_index);

	return sampler;
}

float next_sample1d(inout random_sampler sampler)
{
	return random_float(sampler.generator);
}

float2 next_sample2d(inout random_sampler sampler)
{
	return float2(random_float(sampler.generator), random_float(sampler.generator));
}

#endif