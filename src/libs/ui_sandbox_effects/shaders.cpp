#include "ui_sandbox_effects/shaders.hpp"

#include <string>

namespace ui_sandbox::effects {

std::string imgui_effect_vertex_slang()
{
    return R"slang(
struct VSOutput {
    float2 Frag_UV : TEXCOORD0;
    float4 Frag_Color : COLOR0;
    float4 Position : SV_Position;
};

uniform float4x4 ProjMtx;

[shader("vertex")]
VSOutput vertexMain(
    float2 Position : POSITION0,
    float2 UV : TEXCOORD0,
    float4 Color : COLOR0)
{
    VSOutput output;
    output.Frag_UV = UV;
    output.Frag_Color = Color;
    output.Position = mul(ProjMtx, float4(Position, 0.0, 1.0));
    return output;
}
)slang";
}

std::string make_post_effect_slang(const char* fragment_tail)
{
    return imgui_effect_vertex_slang() + fragment_tail;
}

std::string grayscale_effect_slang()
{
    return make_post_effect_slang(R"slang(
[[vk::binding(0)]] Sampler2D Texture_0;

struct EffectParams {
    float uMix;
    float3 _pad;
};
[[vk::binding(2)]] ConstantBuffer<EffectParams> block_EffectParams_0;

[shader("fragment")]
float4 fragmentMain(
    float2 Frag_UV : TEXCOORD0,
    float4 Frag_Color : COLOR0,
    float4 Position : SV_Position) : SV_Target
{
    float4 texColor = Texture_0.Sample(Frag_UV);
    float4 finalColor = texColor * Frag_Color;
    float gray = dot(finalColor.rgb, float3(0.299, 0.587, 0.114));
    float4 grayCol = float4(gray, gray, gray, finalColor.a);
    float m = saturate(block_EffectParams_0.uMix);
    return float4(lerp(finalColor.rgb, grayCol.rgb, m), finalColor.a);
}
)slang");
}

std::string sepia_effect_slang()
{
    return make_post_effect_slang(R"slang(
[[vk::binding(0)]] Sampler2D Texture_0;

struct EffectParams {
    float uMix;
    float3 _pad;
};
[[vk::binding(2)]] ConstantBuffer<EffectParams> block_EffectParams_0;

[shader("fragment")]
float4 fragmentMain(
    float2 Frag_UV : TEXCOORD0,
    float4 Frag_Color : COLOR0,
    float4 Position : SV_Position) : SV_Target
{
    float4 texColor = Texture_0.Sample(Frag_UV);
    float4 finalColor = texColor * Frag_Color;
    float3 sepia = float3(
        dot(finalColor.rgb, float3(0.393, 0.769, 0.189)),
        dot(finalColor.rgb, float3(0.349, 0.686, 0.168)),
        dot(finalColor.rgb, float3(0.272, 0.534, 0.131)));
    float m = saturate(block_EffectParams_0.uMix);
    return float4(lerp(finalColor.rgb, sepia, m), finalColor.a);
}
)slang");
}

std::string pixelate_effect_slang()
{
    return make_post_effect_slang(R"slang(
[[vk::binding(0)]] Sampler2D Texture_0;

struct EffectParams {
    float uBlocks;
    float3 _pad;
};
[[vk::binding(2)]] ConstantBuffer<EffectParams> block_EffectParams_0;

[shader("fragment")]
float4 fragmentMain(
    float2 Frag_UV : TEXCOORD0,
    float4 Frag_Color : COLOR0,
    float4 Position : SV_Position) : SV_Target
{
    float b = max(block_EffectParams_0.uBlocks, 1.0);
    float2 uv = floor(Frag_UV * b) / b + float2(0.5 / b, 0.5 / b);
    float4 texColor = Texture_0.Sample(uv);
    return texColor * Frag_Color;
}
)slang");
}

std::string chromatic_effect_slang()
{
    return make_post_effect_slang(R"slang(
[[vk::binding(0)]] Sampler2D Texture_0;

struct EffectParams {
    float uStrength;
    float3 _pad;
};
[[vk::binding(2)]] ConstantBuffer<EffectParams> block_EffectParams_0;

[shader("fragment")]
float4 fragmentMain(
    float2 Frag_UV : TEXCOORD0,
    float4 Frag_Color : COLOR0,
    float4 Position : SV_Position) : SV_Target
{
    float s = block_EffectParams_0.uStrength;
    float2 d = float2(s, s * 0.707);
    float4 cM = Texture_0.Sample(Frag_UV);
    float r = Texture_0.Sample(Frag_UV + d).r;
    float b = Texture_0.Sample(Frag_UV - d).b;
    float4 texColor = float4(r, cM.g, b, cM.a);
    return texColor * Frag_Color;
}
)slang");
}

std::string invert_effect_slang()
{
    return make_post_effect_slang(R"slang(
[[vk::binding(0)]] Sampler2D Texture_0;

struct EffectParams {
    float uMix;
    float3 _pad;
};
[[vk::binding(2)]] ConstantBuffer<EffectParams> block_EffectParams_0;

[shader("fragment")]
float4 fragmentMain(
    float2 Frag_UV : TEXCOORD0,
    float4 Frag_Color : COLOR0,
    float4 Position : SV_Position) : SV_Target
{
    float4 texColor = Texture_0.Sample(Frag_UV);
    float4 finalColor = texColor * Frag_Color;
    float3 inv = float3(1.0, 1.0, 1.0) - finalColor.rgb;
    float m = saturate(block_EffectParams_0.uMix);
    return float4(lerp(finalColor.rgb, inv, m), finalColor.a);
}
)slang");
}

std::string scanlines_effect_slang()
{
    return make_post_effect_slang(R"slang(
[[vk::binding(0)]] Sampler2D Texture_0;

struct EffectParams {
    float uIntensity;
    float uLines;
    float2 _pad;
};
[[vk::binding(2)]] ConstantBuffer<EffectParams> block_EffectParams_0;

[shader("fragment")]
float4 fragmentMain(
    float2 Frag_UV : TEXCOORD0,
    float4 Frag_Color : COLOR0,
    float4 Position : SV_Position) : SV_Target
{
    float4 texColor = Texture_0.Sample(Frag_UV);
    float4 finalColor = texColor * Frag_Color;
    float lines = max(block_EffectParams_0.uLines, 1.0);
    float wave = sin(Frag_UV.y * lines * 6.28318530718) * 0.5 + 0.5;
    float att = lerp(1.0 - block_EffectParams_0.uIntensity, 1.0, wave);
    return float4(finalColor.rgb * att, finalColor.a);
}
)slang");
}

std::string figma_fill_effect_slang()
{
    return make_post_effect_slang(R"slang(
[[vk::binding(0)]] Sampler2D Texture_0;
[[vk::binding(1)]] Sampler2D Texture_palette;

struct GradientParams
{
    float2 linearA;
    float2 linearB;
    float2 center;
    float2 ellipse;
    float gtype;
    float replaceMix;
    float angleStart;
    float paletteWidth;
    float spreadMode;       // 0 pad (clamp), 1 repeat, 2 reflect (Figma-style extension)
    float ditherStrength;  // 0..1 screen-space noise on RGB
    float gradM0;
    float gradM1;
    float gradM2;           // guv.x = m0*uv.x + m1*uv.y + m2
    float gradM3;
    float gradM4;
    float gradM5;           // guv.y = m3*uv.x + m4*uv.y + m5
    float paletteLinear;    // 1 if palette tex is RGBA16F with linear RGB, else sRGB-encoded 8-bit
    float _pad0;
    float _pad1;
    float _pad2;
};
[[vk::binding(2)]] ConstantBuffer<GradientParams> block_EffectParams_0;

float2 figmaGradientUVMul(float2 uv, GradientParams p)
{
    return float2(
        p.gradM0 * uv.x + p.gradM1 * uv.y + p.gradM2,
        p.gradM3 * uv.x + p.gradM4 * uv.y + p.gradM5);
}

float applyFigmaSpread(float t, float spreadMode)
{
    int m = int(spreadMode + 0.5);
    if (m <= 0)
        return saturate(t);
    if (m == 1)
        return t - floor(t);
    float v = t - floor(t * 0.5) * 2.0;
    return v > 1.0 ? 2.0 - v : v;
}

float3 linearToSrgb(float3 x)
{
    x = max(x, float3(0, 0, 0));
    float3 lo = x * 12.92;
    float3 hi = 1.055 * pow(x, float3(1.0 / 2.4, 1.0 / 2.4, 1.0 / 2.4)) - 0.055;
    return float3(
        x.r <= 0.0031308 ? lo.r : hi.r,
        x.g <= 0.0031308 ? lo.g : hi.g,
        x.b <= 0.0031308 ? lo.b : hi.b);
}

float4 samplePalette(float t, float paletteWidth)
{
    t = saturate(t);
    float w = max(paletteWidth, 1.0);
    float u = (t * (w - 1.0) + 0.5) / w;
    return Texture_palette.SampleLevel(float2(u, 0.5), 0);
}

float ditherHash(float2 x)
{
    float2 p = frac(x * float2(0.1031, 0.11369));
    p += dot(p, p.yx + 19.19);
    return frac(p.x * p.y);
}

static const float PI = 3.14159265358979323846;

[shader("fragment")]
float4 fragmentMain(
    float2 Frag_UV : TEXCOORD0,
    float4 Frag_Color : COLOR0,
    float4 Position : SV_Position) : SV_Target
{
    float4 texColor = Texture_0.Sample(Frag_UV);
    float4 finalColor = texColor * Frag_Color;
    float3 base = finalColor.rgb;
    float alpha = finalColor.a;

    GradientParams p = block_EffectParams_0;
    float2 guv = figmaGradientUVMul(Frag_UV, p);
    float tRaw = 0.0;
    float g = p.gtype;

    if (g < 0.5)
    {
        float2 ba = p.linearB - p.linearA;
        float denom = dot(ba, ba);
        tRaw = dot(guv - p.linearA, ba) / max(denom, 1e-8);
    }
    else if (g < 1.5)
    {
        float2 q = guv - p.center;
        float2 e = max(p.ellipse, float2(1e-4, 1e-4));
        float2 k = float2(q.x / e.x, q.y / e.y);
        tRaw = length(k);
    }
    else if (g < 2.5)
    {
        float2 d = guv - p.center;
        float ang = atan2(d.y, d.x);
        float u = (ang + PI) / (2.0 * PI);
        float a0 = p.angleStart / (2.0 * PI);
        tRaw = u - a0;
    }
    else
    {
        float2 q = guv - p.center;
        float2 e = max(p.ellipse, float2(1e-4, 1e-4));
        tRaw = (abs(q.x) / e.x + abs(q.y) / e.y);
    }

    float t = applyFigmaSpread(tRaw, p.spreadMode);
    float4 pal = samplePalette(t, p.paletteWidth);
    float3 gfill = (p.paletteLinear > 0.5) ? linearToSrgb(pal.rgb) : pal.rgb;
    float ga = pal.a;
    float m = saturate(p.replaceMix);

    float3 outRgb = lerp(base, gfill, m * ga);
    float outA = lerp(alpha, ga, m);

    float dn = ditherHash(floor(Position.xy)) - 0.5;
    float ds = saturate(p.ditherStrength);
    outRgb = saturate(outRgb + dn * ds * 0.12);

    return float4(outRgb, outA);
}
)slang");
}

} // namespace ui_sandbox::effects
