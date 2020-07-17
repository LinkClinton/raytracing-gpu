#ifndef __RANDOM_GENERATOR_HLSL__
#define __RANDOM_GENERATOR_HLSL__

// lcg random number generator from https://github.com/Twinklebear/ChameleonRT/blob/master/dxr/lcg_rng.hlsl
struct random_generator {
	uint state;
};

uint murmur_hash3_mix(uint hash, uint k)
{
    const uint c1 = 0xcc9e2d51;
    const uint c2 = 0x1b873593;
    const uint r1 = 15;
    const uint r2 = 13;
    const uint m = 5;
    const uint n = 0xe6546b64;

    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;

    hash ^= k;
    hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;

    return hash;
}

uint murmur_hash3_finalize(uint hash)
{
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;

    return hash;
}

uint random_uint(inout random_generator rng)
{
    const uint m = 1664525;
    const uint n = 1013904223;
    rng.state = rng.state * m + n;
    return rng.state;
}

float random_float(inout random_generator rng)
{
    return ldexp((float)random_uint(rng), -32);
}

random_generator create_random_generator(uint sample_index) 
{
    const uint2 dimension = DispatchRaysDimensions().xy;
    const uint2 pixel = DispatchRaysIndex().xy;

    random_generator rng;

    rng.state = murmur_hash3_mix(0, pixel.x + pixel.y * dimension.x);
    rng.state = murmur_hash3_mix(rng.state, sample_index);
    rng.state = murmur_hash3_finalize(rng.state);

    return rng;
}

#endif