//
//  RandomShader.h
//  MetalBasic
//
//  Created by William Lindmeier on 2/20/16.
//
//

#ifndef RandomShader_h
#define RandomShader_h

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct
{
    float4 position [[position]];
    float pointSize [[point_size]];
    float4 color;
    float2 texCoords;
    int texIndex;
} VertOut;

static float4 mod289(float4 x);
static float4 mod289(float4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

static float4 permute(float4 x);
static float4 permute(float4 x)
{
    return mod289(((x*34.0)+1.0)*x);
}

static float4 taylorInvSqrt(float4 r);
static float4 taylorInvSqrt(float4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

static float4 fade(float4 t);
static float4 fade(float4 t)
{
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
static float cnoise(float4 P);
static float cnoise(float4 P)
{
    float4 Pi0 = floor(P); // Integer part for indexing
    float4 Pi1 = Pi0 + 1.0; // Integer part + 1
    Pi0 = mod289(Pi0);
    Pi1 = mod289(Pi1);
    float4 Pf0 = fract(P); // Fractional part for interpolation
    float4 Pf1 = Pf0 - 1.0; // Fractional part - 1.0
    float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    float4 iy = float4(Pi0.yy, Pi1.yy);
    float4 iz0 = float4(Pi0.zzzz);
    float4 iz1 = float4(Pi1.zzzz);
    float4 iw0 = float4(Pi0.wwww);
    float4 iw1 = float4(Pi1.wwww);
    
    float4 ixy = permute(permute(ix) + iy);
    float4 ixy0 = permute(ixy + iz0);
    float4 ixy1 = permute(ixy + iz1);
    float4 ixy00 = permute(ixy0 + iw0);
    float4 ixy01 = permute(ixy0 + iw1);
    float4 ixy10 = permute(ixy1 + iw0);
    float4 ixy11 = permute(ixy1 + iw1);
    
    float4 gx00 = ixy00 * (1.0 / 7.0);
    float4 gy00 = floor(gx00) * (1.0 / 7.0);
    float4 gz00 = floor(gy00) * (1.0 / 6.0);
    gx00 = fract(gx00) - 0.5;
    gy00 = fract(gy00) - 0.5;
    gz00 = fract(gz00) - 0.5;
    float4 gw00 = float4(0.75) - abs(gx00) - abs(gy00) - abs(gz00);
    float4 sw00 = step(gw00, float4(0.0));
    gx00 -= sw00 * (step(0.0, gx00) - 0.5);
    gy00 -= sw00 * (step(0.0, gy00) - 0.5);
    
    float4 gx01 = ixy01 * (1.0 / 7.0);
    float4 gy01 = floor(gx01) * (1.0 / 7.0);
    float4 gz01 = floor(gy01) * (1.0 / 6.0);
    gx01 = fract(gx01) - 0.5;
    gy01 = fract(gy01) - 0.5;
    gz01 = fract(gz01) - 0.5;
    float4 gw01 = float4(0.75) - abs(gx01) - abs(gy01) - abs(gz01);
    float4 sw01 = step(gw01, float4(0.0));
    gx01 -= sw01 * (step(0.0, gx01) - 0.5);
    gy01 -= sw01 * (step(0.0, gy01) - 0.5);
    
    float4 gx10 = ixy10 * (1.0 / 7.0);
    float4 gy10 = floor(gx10) * (1.0 / 7.0);
    float4 gz10 = floor(gy10) * (1.0 / 6.0);
    gx10 = fract(gx10) - 0.5;
    gy10 = fract(gy10) - 0.5;
    gz10 = fract(gz10) - 0.5;
    float4 gw10 = float4(0.75) - abs(gx10) - abs(gy10) - abs(gz10);
    float4 sw10 = step(gw10, float4(0.0));
    gx10 -= sw10 * (step(0.0, gx10) - 0.5);
    gy10 -= sw10 * (step(0.0, gy10) - 0.5);
    
    float4 gx11 = ixy11 * (1.0 / 7.0);
    float4 gy11 = floor(gx11) * (1.0 / 7.0);
    float4 gz11 = floor(gy11) * (1.0 / 6.0);
    gx11 = fract(gx11) - 0.5;
    gy11 = fract(gy11) - 0.5;
    gz11 = fract(gz11) - 0.5;
    float4 gw11 = float4(0.75) - abs(gx11) - abs(gy11) - abs(gz11);
    float4 sw11 = step(gw11, float4(0.0));
    gx11 -= sw11 * (step(0.0, gx11) - 0.5);
    gy11 -= sw11 * (step(0.0, gy11) - 0.5);
    
    float4 g0000 = float4(gx00.x,gy00.x,gz00.x,gw00.x);
    float4 g1000 = float4(gx00.y,gy00.y,gz00.y,gw00.y);
    float4 g0100 = float4(gx00.z,gy00.z,gz00.z,gw00.z);
    float4 g1100 = float4(gx00.w,gy00.w,gz00.w,gw00.w);
    float4 g0010 = float4(gx10.x,gy10.x,gz10.x,gw10.x);
    float4 g1010 = float4(gx10.y,gy10.y,gz10.y,gw10.y);
    float4 g0110 = float4(gx10.z,gy10.z,gz10.z,gw10.z);
    float4 g1110 = float4(gx10.w,gy10.w,gz10.w,gw10.w);
    float4 g0001 = float4(gx01.x,gy01.x,gz01.x,gw01.x);
    float4 g1001 = float4(gx01.y,gy01.y,gz01.y,gw01.y);
    float4 g0101 = float4(gx01.z,gy01.z,gz01.z,gw01.z);
    float4 g1101 = float4(gx01.w,gy01.w,gz01.w,gw01.w);
    float4 g0011 = float4(gx11.x,gy11.x,gz11.x,gw11.x);
    float4 g1011 = float4(gx11.y,gy11.y,gz11.y,gw11.y);
    float4 g0111 = float4(gx11.z,gy11.z,gz11.z,gw11.z);
    float4 g1111 = float4(gx11.w,gy11.w,gz11.w,gw11.w);
    
    float4 norm00 = taylorInvSqrt(float4(dot(g0000, g0000), dot(g0100, g0100), dot(g1000, g1000), dot(g1100, g1100)));
    g0000 *= norm00.x;
    g0100 *= norm00.y;
    g1000 *= norm00.z;
    g1100 *= norm00.w;
    
    float4 norm01 = taylorInvSqrt(float4(dot(g0001, g0001), dot(g0101, g0101), dot(g1001, g1001), dot(g1101, g1101)));
    g0001 *= norm01.x;
    g0101 *= norm01.y;
    g1001 *= norm01.z;
    g1101 *= norm01.w;
    
    float4 norm10 = taylorInvSqrt(float4(dot(g0010, g0010), dot(g0110, g0110), dot(g1010, g1010), dot(g1110, g1110)));
    g0010 *= norm10.x;
    g0110 *= norm10.y;
    g1010 *= norm10.z;
    g1110 *= norm10.w;
    
    float4 norm11 = taylorInvSqrt(float4(dot(g0011, g0011), dot(g0111, g0111), dot(g1011, g1011), dot(g1111, g1111)));
    g0011 *= norm11.x;
    g0111 *= norm11.y;
    g1011 *= norm11.z;
    g1111 *= norm11.w;
    
    float n0000 = dot(g0000, Pf0);
    float n1000 = dot(g1000, float4(Pf1.x, Pf0.yzw));
    float n0100 = dot(g0100, float4(Pf0.x, Pf1.y, Pf0.zw));
    float n1100 = dot(g1100, float4(Pf1.xy, Pf0.zw));
    float n0010 = dot(g0010, float4(Pf0.xy, Pf1.z, Pf0.w));
    float n1010 = dot(g1010, float4(Pf1.x, Pf0.y, Pf1.z, Pf0.w));
    float n0110 = dot(g0110, float4(Pf0.x, Pf1.yz, Pf0.w));
    float n1110 = dot(g1110, float4(Pf1.xyz, Pf0.w));
    float n0001 = dot(g0001, float4(Pf0.xyz, Pf1.w));
    float n1001 = dot(g1001, float4(Pf1.x, Pf0.yz, Pf1.w));
    float n0101 = dot(g0101, float4(Pf0.x, Pf1.y, Pf0.z, Pf1.w));
    float n1101 = dot(g1101, float4(Pf1.xy, Pf0.z, Pf1.w));
    float n0011 = dot(g0011, float4(Pf0.xy, Pf1.zw));
    float n1011 = dot(g1011, float4(Pf1.x, Pf0.y, Pf1.zw));
    float n0111 = dot(g0111, float4(Pf0.x, Pf1.yzw));
    float n1111 = dot(g1111, Pf1);
    
    float4 fade_xyzw = fade(Pf0);
    float4 n_0w = mix(float4(n0000, n1000, n0100, n1100), float4(n0001, n1001, n0101, n1101), fade_xyzw.w);
    float4 n_1w = mix(float4(n0010, n1010, n0110, n1110), float4(n0011, n1011, n0111, n1111), fade_xyzw.w);
    float4 n_zw = mix(n_0w, n_1w, fade_xyzw.z);
    float2 n_yzw = mix(n_zw.xy, n_zw.zw, fade_xyzw.y);
    float n_xyzw = mix(n_yzw.x, n_yzw.y, fade_xyzw.x);
    return 2.2 * n_xyzw;
}

static float pnoise(float4 P, float4 rep);
static float pnoise(float4 P, float4 rep)
{
    float4 Pi0 = fmod(floor(P), rep); // Integer part modulo rep
    float4 Pi1 = fmod(Pi0 + 1.0, rep); // Integer part + 1 mod rep
    Pi0 = mod289(Pi0);
    Pi1 = mod289(Pi1);
    float4 Pf0 = fract(P); // Fractional part for interpolation
    float4 Pf1 = Pf0 - 1.0; // Fractional part - 1.0
    float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    float4 iy = float4(Pi0.yy, Pi1.yy);
    float4 iz0 = float4(Pi0.zzzz);
    float4 iz1 = float4(Pi1.zzzz);
    float4 iw0 = float4(Pi0.wwww);
    float4 iw1 = float4(Pi1.wwww);
    
    float4 ixy = permute(permute(ix) + iy);
    float4 ixy0 = permute(ixy + iz0);
    float4 ixy1 = permute(ixy + iz1);
    float4 ixy00 = permute(ixy0 + iw0);
    float4 ixy01 = permute(ixy0 + iw1);
    float4 ixy10 = permute(ixy1 + iw0);
    float4 ixy11 = permute(ixy1 + iw1);
    
    float4 gx00 = ixy00 * (1.0 / 7.0);
    float4 gy00 = floor(gx00) * (1.0 / 7.0);
    float4 gz00 = floor(gy00) * (1.0 / 6.0);
    gx00 = fract(gx00) - 0.5;
    gy00 = fract(gy00) - 0.5;
    gz00 = fract(gz00) - 0.5;
    float4 gw00 = float4(0.75) - abs(gx00) - abs(gy00) - abs(gz00);
    float4 sw00 = step(gw00, float4(0.0));
    gx00 -= sw00 * (step(0.0, gx00) - 0.5);
    gy00 -= sw00 * (step(0.0, gy00) - 0.5);
    
    float4 gx01 = ixy01 * (1.0 / 7.0);
    float4 gy01 = floor(gx01) * (1.0 / 7.0);
    float4 gz01 = floor(gy01) * (1.0 / 6.0);
    gx01 = fract(gx01) - 0.5;
    gy01 = fract(gy01) - 0.5;
    gz01 = fract(gz01) - 0.5;
    float4 gw01 = float4(0.75) - abs(gx01) - abs(gy01) - abs(gz01);
    float4 sw01 = step(gw01, float4(0.0));
    gx01 -= sw01 * (step(0.0, gx01) - 0.5);
    gy01 -= sw01 * (step(0.0, gy01) - 0.5);
    
    float4 gx10 = ixy10 * (1.0 / 7.0);
    float4 gy10 = floor(gx10) * (1.0 / 7.0);
    float4 gz10 = floor(gy10) * (1.0 / 6.0);
    gx10 = fract(gx10) - 0.5;
    gy10 = fract(gy10) - 0.5;
    gz10 = fract(gz10) - 0.5;
    float4 gw10 = float4(0.75) - abs(gx10) - abs(gy10) - abs(gz10);
    float4 sw10 = step(gw10, float4(0.0));
    gx10 -= sw10 * (step(0.0, gx10) - 0.5);
    gy10 -= sw10 * (step(0.0, gy10) - 0.5);
    
    float4 gx11 = ixy11 * (1.0 / 7.0);
    float4 gy11 = floor(gx11) * (1.0 / 7.0);
    float4 gz11 = floor(gy11) * (1.0 / 6.0);
    gx11 = fract(gx11) - 0.5;
    gy11 = fract(gy11) - 0.5;
    gz11 = fract(gz11) - 0.5;
    float4 gw11 = float4(0.75) - abs(gx11) - abs(gy11) - abs(gz11);
    float4 sw11 = step(gw11, float4(0.0));
    gx11 -= sw11 * (step(0.0, gx11) - 0.5);
    gy11 -= sw11 * (step(0.0, gy11) - 0.5);
    
    float4 g0000 = float4(gx00.x,gy00.x,gz00.x,gw00.x);
    float4 g1000 = float4(gx00.y,gy00.y,gz00.y,gw00.y);
    float4 g0100 = float4(gx00.z,gy00.z,gz00.z,gw00.z);
    float4 g1100 = float4(gx00.w,gy00.w,gz00.w,gw00.w);
    float4 g0010 = float4(gx10.x,gy10.x,gz10.x,gw10.x);
    float4 g1010 = float4(gx10.y,gy10.y,gz10.y,gw10.y);
    float4 g0110 = float4(gx10.z,gy10.z,gz10.z,gw10.z);
    float4 g1110 = float4(gx10.w,gy10.w,gz10.w,gw10.w);
    float4 g0001 = float4(gx01.x,gy01.x,gz01.x,gw01.x);
    float4 g1001 = float4(gx01.y,gy01.y,gz01.y,gw01.y);
    float4 g0101 = float4(gx01.z,gy01.z,gz01.z,gw01.z);
    float4 g1101 = float4(gx01.w,gy01.w,gz01.w,gw01.w);
    float4 g0011 = float4(gx11.x,gy11.x,gz11.x,gw11.x);
    float4 g1011 = float4(gx11.y,gy11.y,gz11.y,gw11.y);
    float4 g0111 = float4(gx11.z,gy11.z,gz11.z,gw11.z);
    float4 g1111 = float4(gx11.w,gy11.w,gz11.w,gw11.w);
    
    float4 norm00 = taylorInvSqrt(float4(dot(g0000, g0000), dot(g0100, g0100), dot(g1000, g1000), dot(g1100, g1100)));
    g0000 *= norm00.x;
    g0100 *= norm00.y;
    g1000 *= norm00.z;
    g1100 *= norm00.w;
    
    float4 norm01 = taylorInvSqrt(float4(dot(g0001, g0001), dot(g0101, g0101), dot(g1001, g1001), dot(g1101, g1101)));
    g0001 *= norm01.x;
    g0101 *= norm01.y;
    g1001 *= norm01.z;
    g1101 *= norm01.w;
    
    float4 norm10 = taylorInvSqrt(float4(dot(g0010, g0010), dot(g0110, g0110), dot(g1010, g1010), dot(g1110, g1110)));
    g0010 *= norm10.x;
    g0110 *= norm10.y;
    g1010 *= norm10.z;
    g1110 *= norm10.w;
    
    float4 norm11 = taylorInvSqrt(float4(dot(g0011, g0011), dot(g0111, g0111), dot(g1011, g1011), dot(g1111, g1111)));
    g0011 *= norm11.x;
    g0111 *= norm11.y;
    g1011 *= norm11.z;
    g1111 *= norm11.w;
    
    float n0000 = dot(g0000, Pf0);
    float n1000 = dot(g1000, float4(Pf1.x, Pf0.yzw));
    float n0100 = dot(g0100, float4(Pf0.x, Pf1.y, Pf0.zw));
    float n1100 = dot(g1100, float4(Pf1.xy, Pf0.zw));
    float n0010 = dot(g0010, float4(Pf0.xy, Pf1.z, Pf0.w));
    float n1010 = dot(g1010, float4(Pf1.x, Pf0.y, Pf1.z, Pf0.w));
    float n0110 = dot(g0110, float4(Pf0.x, Pf1.yz, Pf0.w));
    float n1110 = dot(g1110, float4(Pf1.xyz, Pf0.w));
    float n0001 = dot(g0001, float4(Pf0.xyz, Pf1.w));
    float n1001 = dot(g1001, float4(Pf1.x, Pf0.yz, Pf1.w));
    float n0101 = dot(g0101, float4(Pf0.x, Pf1.y, Pf0.z, Pf1.w));
    float n1101 = dot(g1101, float4(Pf1.xy, Pf0.z, Pf1.w));
    float n0011 = dot(g0011, float4(Pf0.xy, Pf1.zw));
    float n1011 = dot(g1011, float4(Pf1.x, Pf0.y, Pf1.zw));
    float n0111 = dot(g0111, float4(Pf0.x, Pf1.yzw));
    float n1111 = dot(g1111, Pf1);
    
    float4 fade_xyzw = fade(Pf0);
    float4 n_0w = mix(float4(n0000, n1000, n0100, n1100), float4(n0001, n1001, n0101, n1101), fade_xyzw.w);
    float4 n_1w = mix(float4(n0010, n1010, n0110, n1110), float4(n0011, n1011, n0111, n1111), fade_xyzw.w);
    float4 n_zw = mix(n_0w, n_1w, fade_xyzw.z);
    float2 n_yzw = mix(n_zw.xy, n_zw.zw, fade_xyzw.y);
    float n_xyzw = mix(n_yzw.x, n_yzw.y, fade_xyzw.x);
    return 2.2 * n_xyzw;
}

float3 rgb2hsv(float3 c);
float3 rgb2hsv(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = c.g < c.b ? float4(c.bg, K.wz) : float4(c.gb, K.xy);
    float4 q = c.r < p.x ? float4(p.xyw, c.r) : float4(c.r, p.yzx);
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

float3 hsv2rgb(float3 c);
float3 hsv2rgb(float3 c)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

static matrix_float4x4 rotationMatrix( const matrix_float4x4 m );
static matrix_float4x4 rotationMatrix( const matrix_float4x4 m )
{
    return matrix_float4x4((vector_float4){m[0][0], m[0][1], m[0][2], 0},
                           (vector_float4){m[1][0], m[1][1], m[1][2], 0},
                           (vector_float4){m[2][0], m[2][1], m[2][2], 0},
                           (vector_float4){0,       0,       0,       1} );
}

#endif /* RandomShader_h */
