#ifndef __TYPES_HLSL__
#define __TYPES_HLSL__

#include "macros.hlsl"

// fix resharper c++ error or miss type
#ifndef __HLSL_SHADER__

#define uint64_t uint
#define int64_t int

enum COMMITTED_STATUS : uint
{
    COMMITTED_NOTHING,
    COMMITTED_TRIANGLE_HIT,
    COMMITTED_PROCEDURAL_PRIMITIVE_HIT
};

struct RayQuery
{
    void TraceRayInline(
		RaytracingAccelerationStructure AccelerationStructure,
		uint RayFlags,
		uint InstanceInclusionMask,
		RayDesc Ray);

    void Proceed();

    COMMITTED_STATUS CommittedStatus();

    float CommittedRayT();

    uint CommittedInstanceID();
};

#define RAY_QUERY(flags) RayQuery

#else

#define RAY_QUERY(flags) RayQuery<flags>

#endif

// resource define

#define SHADER_RESOURCE_DEFINE(type, name, base, space) type name : register(base, space)

// hash functions from https://github.com/ospray/ospray
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

// pcg from https://www.pcg-random.org/
struct pcg32_random
{
    uint64_t state;
    uint64_t inc;

    void initialize(uint64_t new_state, uint64_t new_inc);

    uint next_uint();

    float next_float();
};

void pcg32_random::initialize(uint64_t new_state, uint64_t new_inc)
{
    state = 0;
    inc = (new_inc << 1u) | 1u;

    state = murmur_hash3_mix((uint) state, (uint)new_state);
    state = murmur_hash3_finalize((uint)state);

    next_uint();
    state = state + new_state;
    next_uint();
}

uint pcg32_random::next_uint()
{
	uint64_t old_state = state;
    // Advance internal state
    state = old_state * 6364136223846793005ULL + (inc | 1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint xorshifted = (uint) (((old_state >> 18u) ^ old_state) >> 27u);
    uint rot = (uint) old_state >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

float pcg32_random::next_float()
{
    return ldexp((float)next_uint(), -32);
}


#endif
