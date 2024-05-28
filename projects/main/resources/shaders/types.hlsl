#ifndef __TYPES_HLSL__
#define __TYPES_HLSL__

#include "macros.hlsl"

// fix resharper c++ error or miss type
#ifndef __HLSL_SHADER__

#define uint64_t uint
#define int64_t int

#define RAY_FLAG_FORCE_OPAQUE 0

#else

#endif

// resource define

#define SHADER_RESOURCE_DEFINE(type, name, base, space) type name : register(base, space)

// const variable define
#define ONE_OVER_PI 0.318309886183790671537767526745028724
#define ONE_OVER_TWO_PI 0.159154943091895335768883763372514362
#define QUARTER_PI 0.785398163397448309615660845819875721
#define HALF_PI 1.57079632679489661923132169163975144
#define TWO_PI 6.28318530717958647692528676655900576
#define ONE_PI 3.14159265358979323846264338327950288

// math macros

#define MAX(x, y) (x > y ? x : y)

struct texture_info
{
    float3 scale;
    uint index;
};

#endif
