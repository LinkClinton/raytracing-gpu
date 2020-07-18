#ifndef __FIX_RESHARPER_ERROR__
#define __FIX_RESHARPER_ERROR__

#ifndef __HLSL_SHADER__

using uint = unsigned;

template <typename T>
struct any1 {
	T x;
};

template <typename T>
struct any2 {
	T x, y;
};

template <typename T>
struct any3 {
	T x, y, z;
};

template <typename T>
struct any4 {
	T x, y, z, w;
};

using uint1 = any1<uint>;
using uint2 = any2<uint>;
using uint3 = any3<uint>;
using uint4 = any4<uint>;

using float1 = any1<float>;
using float2 = any2<float>;
using float3 = any3<float>;
using float4 = any4<float>;

struct RayDesc {
	float3 Direction;
	float3 Origin;
	float TMin;
	float TMax;
};

template <typename T>
T abs(T value) { return value; }

template <typename T>
T cos(T value) { return value; }

template <typename T>
T sin(T value) { return value; }

template <typename T>
T sqrt(T value) { return value; }

template <typename T>
float dot(T x, T y) { return x; }

template <typename T>
T pow(T x, float v) { return x; }

float max(float x, float y) { return x; }


#endif

#endif