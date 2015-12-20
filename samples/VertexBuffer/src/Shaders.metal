//
//  Shaders.metal
//  MetalBasic
//
//  Created by William Lindmeier on 10/11/15.
//  Copyright (c) 2015 wdlindmeier. All rights reserved.
//

#include <metal_stdlib>
#include <simd/simd.h>
#include "MetalConstants.h"
#include "SharedData.h"

using namespace metal;
using namespace cinder::mtl;

// Variables in constant address space
constant float3 light_position = float3(0.0, 1.0, -1.0);
constant float4 ambient_color  = float4(0.18, 0.24, 0.8, 1.0);
constant float4 diffuse_color  = float4(0.4, 0.4, 1.0, 1.0);
// NOTE: samplers defined in the shader don't appear to have an anisotropy param
constexpr sampler shaderSampler( coord::normalized, // normalized (0-1) or coord::pixel (0-width,height)
                                 address::repeat, // repeat, clamp_to_zero, clamp_to_edge,
                                 filter::linear, // nearest or linear
                                 mip_filter::linear ); // nearest or linear or none

typedef struct
{
    packed_float3 position;
    packed_float3 normal;
} InterleavedVertex;

typedef struct
{
    float4 position [[position]];
    float4 color;
    float2 texCoords;
} ColorInOut;

// Vertex shader function
vertex ColorInOut lighting_vertex_interleaved( device const InterleavedVertex* vertex_array [[ buffer(ciBufferIndexInterleavedVerts) ]],
                                               constant ciUniforms_t& uniforms [[ buffer(ciBufferIndexUniforms) ]],
                                               unsigned int vid [[ vertex_id ]] )
{
    ColorInOut out;
    
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    out.position = uniforms.modelViewProjectionMatrix * in_position;
    
    float3 normal = vertex_array[vid].normal;
    float4 eye_normal = normalize(uniforms.normalMatrix * float4(normal, 0.0));
    float n_dot_l = dot(eye_normal.rgb, normalize(light_position));
    n_dot_l = fmax(0.0, n_dot_l);
    
    out.color = ambient_color + diffuse_color * n_dot_l;

    return out;
}

// Vertex Bhader using an interleaved geom::Source
// CubeVertex is found in SharedData.h
vertex ColorInOut lighting_vertex_interleaved_src( device const CubeVertex* verts [[ buffer(ciBufferIndexInterleavedVerts) ]],
                                                   device const uint* indices [[ buffer(ciBufferIndexIndicies) ]],
                                                   constant ciUniforms_t& uniforms [[ buffer(ciBufferIndexUniforms) ]],
                                                   unsigned int vid [[ vertex_id ]] )
{
    ColorInOut out;
    
    const uint idx = indices[vid];
    CubeVertex vert = verts[idx];
    float4 in_position = float4(vert.position, 1.0);
    out.position = uniforms.modelViewProjectionMatrix * in_position;
    
    float4 eye_normal = normalize(uniforms.normalMatrix * float4(vert.normal, 0.0));
    float n_dot_l = dot(eye_normal.rgb, normalize(light_position));
    n_dot_l = fmax(0.0, n_dot_l);
    
    out.texCoords = vert.texCoord0;
    out.color = n_dot_l;
    
    return out;
}

// Vertex Buffer using attrib buffers
vertex ColorInOut lighting_vertex_attrib_buffers( device const packed_float3* positions [[ buffer(ciBufferIndexPositions) ]],
                                                  device const packed_float3* normals [[ buffer(ciBufferIndexNormals) ]],
                                                  constant ciUniforms_t& uniforms [[ buffer(ciBufferIndexUniforms) ]],
                                                  unsigned int vid [[ vertex_id ]] )
{
    ColorInOut out;
    
    float4 in_position = float4(positions[vid], 1.0);
    out.position = uniforms.modelViewProjectionMatrix * in_position;
    
    float3 normal = normals[vid];
    float4 eye_normal = normalize(uniforms.normalMatrix * float4(normal, 0.0));
    float n_dot_l = dot(eye_normal.rgb, normalize(light_position));
    n_dot_l = fmax(0.0, n_dot_l);
    
    out.color = ambient_color + diffuse_color * n_dot_l;

    return out;
}

// Fragment shader function
fragment float4 lighting_texture_fragment( ColorInOut in [[ stage_in ]],
                                           texture2d<float> textureCube [[ texture(ciTextureIndex0) ]],
                                           sampler objcSampler [[ sampler(ciSamplerIndex0) ]] )
{
    // Use the shader sampler
    // float4 texColor = textureCube.sample(shaderSampler, in.texCoords);
    // Use the sampler passed in from the app:
    float4 texColor = textureCube.sample(objcSampler, in.texCoords);
    return float4(texColor.rgb * in.color.rgb, texColor.a);
}

fragment float4 lighting_fragment( ColorInOut in [[stage_in]] )
{
    return in.color;
}

