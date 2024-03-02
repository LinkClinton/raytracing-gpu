#ifndef __DYNAMIC_TEXTURE_HEAP_HLSL__
#define __DYNAMIC_TEXTURE_HEAP_HLSL__

#include "../types.hlsl"

struct dynamic_texture_heap
{
    template <typename TextureType>
    static TextureType get(uint index);

    template <typename ElementType>
    static ElementType sample_level(uint index, SamplerState sampler, float2 uv, float lod);
};

template <typename TextureType>
TextureType dynamic_texture_heap::get(uint index)
{
    return ResourceDescriptorHeap[NonUniformResourceIndex(index)];
}

template <typename ElementType>
ElementType dynamic_texture_heap::sample_level(uint index, SamplerState sampler, float2 uv, float lod)
{
    return index != INDEX_NULL ? get<Texture2D<ElementType> >(index).SampleLevel(sampler, uv, lod) : 1;
}

#endif
