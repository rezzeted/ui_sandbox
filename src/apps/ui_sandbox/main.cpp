#define NOMINMAX
#include <drui/drui.h>
#include "editor_layout.h"
#include "panels.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3_slang.h"
#include "imgui_effect_helpers.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <numeric>
#include <string>
#include <vector>

static void glfw_error_cb(int err, const char* desc) {
    std::fprintf(stderr, "GLFW error %d: %s\n", err, desc);
}

// Shared ImGui-compatible vertex stage for all custom effects (std140 UBO at binding 2).
static const char* ImGuiEffectVertexSlang()
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

static std::string MakePostEffectSlang(const char* fragmentTail)
{
    return std::string(ImGuiEffectVertexSlang()) + fragmentTail;
}

static std::string GrayscaleEffectSlang()
{
    return MakePostEffectSlang(R"slang(
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

// Sepia tint (classic matrix), strength uMix.
static std::string SepiaEffectSlang()
{
    return MakePostEffectSlang(R"slang(
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

// Mosaic: uBlocks = cells along U (same along V).
static std::string PixelateEffectSlang()
{
    return MakePostEffectSlang(R"slang(
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

// Subtle CA: shift R/B in UV; uStrength in UV units (e.g. 0.002..0.02).
static std::string ChromaticEffectSlang()
{
    return MakePostEffectSlang(R"slang(
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

static std::string InvertEffectSlang()
{
    return MakePostEffectSlang(R"slang(
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

// Horizontal scanlines; uIntensity darkening, uLines = band count across 0..1 UV height.
static std::string ScanlinesEffectSlang()
{
    return MakePostEffectSlang(R"slang(
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

/// Figma gradient types in UV 0..1: Linear (two handles), Radial (ellipse), Angular (conic), Diamond (Manhattan / L1).
/// Stops in `[[vk::binding(1)]] Texture_palette`; spread / UV affine / dither / linear palette sampling (std140 UBO).
static std::string FigmaFillEffectSlang()
{
    return MakePostEffectSlang(R"slang(
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

/// Must match `FigmaFillEffectSlang` GradientParams (std140).
struct FigmaUnifiedGradientUBO
{
    float linearAx, linearAy, linearBx, linearBy;
    float centerX, centerY, ellipseX, ellipseY;
    float gtype;
    float replaceMix;
    float angleStart;
    float paletteWidth;
    float spreadMode;
    float ditherStrength;
    float gradM0, gradM1, gradM2, gradM3, gradM4, gradM5;
    float paletteLinear;
    float _pad0, _pad1, _pad2;
};
static_assert(sizeof(FigmaUnifiedGradientUBO) == 96, "Figma UBO size mismatch (std140 vs Slang)");

static constexpr int kFigmaMaxStops = 16;
static constexpr int kFigmaPaletteTexWidth = 1024;

#ifndef GL_RGBA16F
#define GL_RGBA16F 0x881A
#endif

static float FigmaSrgbChannelToLinear(float c)
{
    c = std::clamp(c, 0.f, 1.f);
    return (c <= 0.04045f) ? (c / 12.92f)
                           : std::pow((c + 0.055f) / 1.055f, 2.4f);
}

static float FigmaLinearChannelToSrgb(float l)
{
    l = std::clamp(l, 0.f, 1.f);
    return (l <= 0.0031308f) ? (l * 12.92f)
                            : (1.055f * std::pow(l, 1.f / 2.4f) - 0.055f);
}

/// UV affine: gradient space = T(0.5)*R(deg)*S(sx,sy)*T(-0.5) * T(tx,ty) applied to ImGui UV.
static void FigmaBuildGradientAffine(float rotDeg, float scaleX, float scaleY, float tx, float ty, float* m6)
{
    const float rad = rotDeg * (3.14159265358979323846f / 180.f);
    const float c = std::cos(rad);
    const float s = std::sin(rad);
    const float ox = 0.5f;
    const float oy = 0.5f;
    const float m00 = c * scaleX;
    const float m01 = -s * scaleY;
    const float m10 = s * scaleX;
    const float m11 = c * scaleY;
    m6[0] = m00;
    m6[1] = m01;
    m6[2] = ox - m00 * ox - m01 * oy + tx;
    m6[3] = m10;
    m6[4] = m11;
    m6[5] = oy - m10 * ox - m11 * oy + ty;
}

static bool FigmaProbeRgba16fTexture()
{
    GLuint tid = 0;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 1, 0, GL_RGBA, GL_FLOAT, nullptr);
    const GLenum err = glGetError();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &tid);
    return err == GL_NO_ERROR;
}

static void RebuildFigmaPaletteTexture(GLuint tex, int texWidth, int nStops, const float colors[][4],
                                       const float* positionsRaw, bool storeLinearRgba16f)
{
    if (tex == 0 || nStops < 2 || texWidth < 2)
        return;

    std::vector<int> order(static_cast<size_t>(nStops));
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(),
              [&](int a, int b) { return positionsRaw[a] < positionsRaw[b]; });

    std::vector<float> pos(static_cast<size_t>(nStops));
    std::vector<std::array<float, 4>> colsLin(static_cast<size_t>(nStops));
    for (int i = 0; i < nStops; ++i) {
        pos[static_cast<size_t>(i)] = std::clamp(positionsRaw[order[static_cast<size_t>(i)]], 0.f, 1.f);
        const float* src = colors[order[static_cast<size_t>(i)]];
        colsLin[static_cast<size_t>(i)][0] = FigmaSrgbChannelToLinear(src[0]);
        colsLin[static_cast<size_t>(i)][1] = FigmaSrgbChannelToLinear(src[1]);
        colsLin[static_cast<size_t>(i)][2] = FigmaSrgbChannelToLinear(src[2]);
        colsLin[static_cast<size_t>(i)][3] = std::clamp(src[3], 0.f, 1.f);
    }

    auto sampleStopsLinear = [&](float t) -> std::array<float, 4> {
        t = std::clamp(t, 0.f, 1.f);
        if (t <= pos.front())
            return colsLin[0];
        if (t >= pos.back())
            return colsLin.back();
        for (int i = 0; i < nStops - 1; ++i) {
            if (t <= pos[static_cast<size_t>(i + 1)]) {
                const float t0 = pos[static_cast<size_t>(i)];
                const float t1 = pos[static_cast<size_t>(i + 1)];
                const float u = (t - t0) / std::max(t1 - t0, 1e-8f);
                const auto& a = colsLin[static_cast<size_t>(i)];
                const auto& b = colsLin[static_cast<size_t>(i + 1)];
                return { a[0] + (b[0] - a[0]) * u, a[1] + (b[1] - a[1]) * u, a[2] + (b[2] - a[2]) * u,
                         a[3] + (b[3] - a[3]) * u };
            }
        }
        return colsLin.back();
    };

    glBindTexture(GL_TEXTURE_2D, tex);
    if (storeLinearRgba16f) {
        std::vector<float> pix(static_cast<size_t>(texWidth) * 4u);
        for (int x = 0; x < texWidth; ++x) {
            const float t =
                (texWidth <= 1) ? 0.f : static_cast<float>(x) / static_cast<float>(texWidth - 1);
            const auto p = sampleStopsLinear(t);
            pix[static_cast<size_t>(x) * 4u + 0u] = p[0];
            pix[static_cast<size_t>(x) * 4u + 1u] = p[1];
            pix[static_cast<size_t>(x) * 4u + 2u] = p[2];
            pix[static_cast<size_t>(x) * 4u + 3u] = p[3];
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texWidth, 1, 0, GL_RGBA, GL_FLOAT, pix.data());
    } else {
        std::vector<uint8_t> pix(static_cast<size_t>(texWidth) * 4u);
        for (int x = 0; x < texWidth; ++x) {
            const float t =
                (texWidth <= 1) ? 0.f : static_cast<float>(x) / static_cast<float>(texWidth - 1);
            const auto lin = sampleStopsLinear(t);
            pix[static_cast<size_t>(x) * 4u + 0u] =
                static_cast<uint8_t>(std::clamp(FigmaLinearChannelToSrgb(lin[0]) * 255.f, 0.f, 255.f));
            pix[static_cast<size_t>(x) * 4u + 1u] =
                static_cast<uint8_t>(std::clamp(FigmaLinearChannelToSrgb(lin[1]) * 255.f, 0.f, 255.f));
            pix[static_cast<size_t>(x) * 4u + 2u] =
                static_cast<uint8_t>(std::clamp(FigmaLinearChannelToSrgb(lin[2]) * 255.f, 0.f, 255.f));
            pix[static_cast<size_t>(x) * 4u + 3u] =
                static_cast<uint8_t>(std::clamp(lin[3] * 255.f, 0.f, 255.f));
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix.data());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static void RegisterDemoEffects(ImGuiRenderUX::EffectSystem& effectSystem)
{
    auto reg = [&](const char* name, const std::string& src) {
        ImGuiRenderUX::EffectCreateDesc desc;
        desc.name = name;
        desc.shaderSource = src;
        desc.vertexEntry = "vertexMain";
        desc.fragmentEntry = "fragmentMain";
        desc.blendMode = ImGuiRenderUX::BuiltinBlendMode::Alpha;
        std::string err;
        ImGuiRenderUX::EffectHandle h = effectSystem.EnsureEffect(desc, &err);
        if (!h.IsValid())
            std::fprintf(stderr, "Effect '%s': %s\n", name, err.c_str());
        return h;
    };

    ImGuiRenderUX::EffectHandle g = reg("grayscale", GrayscaleEffectSlang());
    if (g.IsValid())
        effectSystem.ExpectEffectUniformBytes(g, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle s = reg("sepia", SepiaEffectSlang());
    if (s.IsValid())
        effectSystem.ExpectEffectUniformBytes(s, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle p = reg("pixelate", PixelateEffectSlang());
    if (p.IsValid())
        effectSystem.ExpectEffectUniformBytes(p, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle c = reg("chromatic", ChromaticEffectSlang());
    if (c.IsValid())
        effectSystem.ExpectEffectUniformBytes(c, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle i = reg("invert", InvertEffectSlang());
    if (i.IsValid())
        effectSystem.ExpectEffectUniformBytes(i, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle sc = reg("scanlines", ScanlinesEffectSlang());
    if (sc.IsValid())
        effectSystem.ExpectEffectUniformBytes(sc, sizeof(float) * 4);

    ImGuiRenderUX::EffectHandle ff = reg("figma_fill", FigmaFillEffectSlang());
    if (ff.IsValid())
        effectSystem.ExpectEffectUniformBytes(ff, sizeof(FigmaUnifiedGradientUBO));
}

/// Draw three tinted quads with `tex` for effect preview (inside an EffectDrawRegionScope).
static void DrawEffectPreviewQuads(ImDrawList* dl, ImTextureID tex, ImVec2 p0, float sz, float gap)
{
    const ImU32 cols[3] = { IM_COL32(255, 90, 90, 255), IM_COL32(90, 255, 120, 255), IM_COL32(120, 140, 255, 255) };
    for (int k = 0; k < 3; ++k)
    {
        float x0 = p0.x + k * (sz + gap);
        dl->AddImage(tex, ImVec2(x0, p0.y), ImVec2(x0 + sz, p0.y + sz), ImVec2(0, 0), ImVec2(1, 1), cols[k]);
    }
    ImGui::Dummy(ImVec2(3.f * sz + 2.f * gap, sz));
}

int main() {
    glfwSetErrorCallback(glfw_error_cb);
    if (!glfwInit()) return 1;

#ifdef __APPLE__
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1600, 1000, "UI Sandbox", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    if (gl3wInit() != 0) {
        std::fprintf(stderr, "Failed to initialize OpenGL loader (gl3w)\n");
        glfwTerminate();
        return 1;
    }
    const bool figma_palette_16f_ok = FigmaProbeRgba16fTexture();
    glfwSwapInterval(1);

    float dpi_x = 1.0f, dpi_y = 1.0f;
    glfwGetWindowContentScale(window, &dpi_x, &dpi_y);
    const float content_scale = std::clamp(std::max(dpi_x, dpi_y), 1.0f, 3.0f);

    int fb_w = 0, win_w = 0;
    glfwGetFramebufferSize(window, &fb_w, nullptr);
    glfwGetWindowSize(window, &win_w, nullptr);
    const float fb_scale = (win_w > 0) ? static_cast<float>(fb_w) / static_cast<float>(win_w) : 1.0f;

    const float font_scale = content_scale;
    const float dpi_scale = content_scale / fb_scale;
    const float gap = 8.0f * dpi_scale;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Floatable ImGui windows become real GLFW/OS windows when dragged outside the main viewport
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImFont* default_font = DrUI::SetupFonts(io, font_scale);
    if (default_font) io.FontDefault = default_font;
    ImGui::GetStyle().FontScaleMain = 1.0f / fb_scale;

    DrUI::ApplyTheme(DrUI::ThemeId::Dark, dpi_scale);

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3Slang_Init(glsl_version);

    ImGuiRenderUX::EffectSystem effectSystem;
    if (!effectSystem.Initialize()) {
        std::fprintf(stderr, "EffectSystem init failed\n");
    }
    ImGuiRenderUX::BuiltinGpuTextures builtinTextures;
    builtinTextures.EnsureWhite1x1();
    ImTextureID whiteTexImId = builtinTextures.White1x1();

    RegisterDemoEffects(effectSystem);

    GLuint figma_palette_gl = 0;
    glGenTextures(1, &figma_palette_gl);

    EditorSplitters splitters;
    PanelVisibility panels;
    panels.dpi_scale = dpi_scale;
    bool show_grayscale_window = true;
    bool show_region_demo_window = true;
    bool show_effect_gallery = true;
    bool show_effect_debug = false;
    static float s_grayscaleMix = 1.0f;
    static float s_sepiaMix = 0.85f;
    static float s_pixelateBlocks = 18.f;
    static float s_chromaStrength = 0.006f;
    static float s_invertMix = 0.65f;
    static float s_scanlineIntensity = 0.35f;
    static float s_scanlineLines = 180.f;
    // Figma-style fills: Linear | Radial | Angular | Diamond (UV 0..1); stops baked into GL palette texture.
    static int   s_figmaGtype = 0;
    static int   s_figmaStopCount = 4;
    static float s_figmaReplace = 1.f;
    static float s_figmaLinA[2] = { 0.05f, 0.15f };
    static float s_figmaLinB[2] = { 0.95f, 0.85f };
    static float s_figmaCenter[2] = { 0.5f, 0.5f };
    static float s_figmaEllipse[2] = { 0.5f, 0.5f };
    static float s_figmaAngleDeg = 0.f;
    static float s_figmaC[kFigmaMaxStops][4] = {
        { 0.39f, 0.08f, 0.95f, 1.f },
        { 0.95f, 0.25f, 0.45f, 1.f },
        { 0.98f, 0.75f, 0.2f, 1.f },
        { 0.15f, 0.65f, 1.f, 1.f },
    };
    static float s_figmaS[kFigmaMaxStops] = { 0.f, 0.33f, 0.66f, 1.f };
    static int   s_figmaSpread = 0;
    static float s_figmaDither = 0.35f;
    static float s_figmaFrameRotDeg = 0.f;
    static float s_figmaFrameSx = 1.f;
    static float s_figmaFrameSy = 1.f;
    static float s_figmaFrameTx = 0.f;
    static float s_figmaFrameTy = 0.f;
    static bool  s_figmaPalette16f = true;

    {
        ImGuiRenderUX::EffectHandle palFx = effectSystem.FindEffectByName("figma_fill");
        if (palFx.IsValid())
            effectSystem.SetEffectPaletteTexture(palFx, (ImTextureID)(intptr_t)figma_palette_gl);
        RebuildFigmaPaletteTexture(figma_palette_gl, kFigmaPaletteTexWidth, s_figmaStopCount, s_figmaC, s_figmaS,
                                   figma_palette_16f_ok && s_figmaPalette16f);
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        effectSystem.AdvanceFrame();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        ImGui_ImplOpenGL3Slang_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        effectSystem.NotifyAfterImGuiNewFrame();
        effectSystem.TickAutoReload(ImGui::GetIO().DeltaTime);

        ImGuiRenderUX::EffectHandle grayscaleEffect = effectSystem.FindEffectByName("grayscale");
        ImGuiRenderUX::EffectHandle sepiaEffect = effectSystem.FindEffectByName("sepia");
        ImGuiRenderUX::EffectHandle pixelateEffect = effectSystem.FindEffectByName("pixelate");
        ImGuiRenderUX::EffectHandle chromaticEffect = effectSystem.FindEffectByName("chromatic");
        ImGuiRenderUX::EffectHandle invertEffect = effectSystem.FindEffectByName("invert");
        ImGuiRenderUX::EffectHandle scanlinesEffect = effectSystem.FindEffectByName("scanlines");
        ImGuiRenderUX::EffectHandle figmaFillEffect = effectSystem.FindEffectByName("figma_fill");

        ImGuiViewport* vp = ImGui::GetMainViewport();

        DrawMainMenuBar(panels);

        float status_h = panels.status_bar ? 22.0f * dpi_scale : 0.0f;

        auto layout = CalculateLayout(vp, gap, 0.0f, status_h, splitters);
        HandleSplitters(layout, gap, vp, 0.0f, status_h, splitters);

        DrawLeftPanel(layout.left, dpi_scale, panels);
        DrawCanvasPanel(layout.canvas, dpi_scale);
        DrawSplitterIndicators(layout, gap, splitters);

        if (grayscaleEffect.IsValid() && show_grayscale_window) {
            struct GrayscaleUBO { float uMix; float _pad[3]; };
            IMGUI_EFFECT_UNIFORM_STRUCT(GrayscaleUBO);
            GrayscaleUBO ubo{ s_grayscaleMix, { 0, 0, 0 } };
            effectSystem.SetEffectUniformStruct(grayscaleEffect, ubo);

            ImGuiRenderUX::EffectWindowScope scope(effectSystem, "Grayscale", grayscaleEffect, &show_grayscale_window);
            if (scope) {
                ImGui::TextUnformatted("EffectWindowScope + uniform uMix (std140 binding 2).");
                ImGui::SliderFloat("Grayscale mix", &s_grayscaleMix, 0.0f, 1.0f);
                ImGui::Checkbox("Effect system debug", &show_effect_debug);
                ImGui::Separator();
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                float sz = 60.f * dpi_scale;
                float g = 8.f * dpi_scale;
                DrawEffectPreviewQuads(ImGui::GetWindowDrawList(), whiteTexImId, p0, sz, g);
                ImGui::TextUnformatted("Quads: lerp(color, gray, mix). Text uses font (skipped by default).");
            }
        }

        // Separate window: effect applies only to BeginEffectDrawRegion / EndEffectDrawRegion span (not whole window).
        if (grayscaleEffect.IsValid() && show_region_demo_window) {
            struct GrayscaleUBO { float uMix; float _pad[3]; };
            IMGUI_EFFECT_UNIFORM_STRUCT(GrayscaleUBO);
            GrayscaleUBO ubo{ s_grayscaleMix, { 0, 0, 0 } };
            effectSystem.SetEffectUniformStruct(grayscaleEffect, ubo);
            ImGui::Begin("Grayscale region only", &show_region_demo_window);
            ImGui::TextUnformatted("Only the orange quad below is in the effect region.");
            {
                ImGuiRenderUX::EffectDrawRegionScope regionScope(effectSystem, grayscaleEffect);
                if (regionScope) {
                    ImVec2 p0 = ImGui::GetCursorScreenPos();
                    float sz = 48.f * dpi_scale;
                    ImGui::GetWindowDrawList()->AddImage(whiteTexImId, p0, ImVec2(p0.x + sz, p0.y + sz),
                                                         ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 180, 60, 255));
                    ImGui::Dummy(ImVec2(sz, sz));
                }
            }
            ImGui::TextUnformatted("This line is outside the region (no effect pass on font).");
            ImGui::End();
        }

        // Gallery: one scrollable window with a row per post-process (each row is its own EffectDrawRegionScope).
        if (show_effect_gallery) {
            ImGui::SetNextWindowSize(ImVec2(420.f * dpi_scale, 520.f * dpi_scale), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Effect gallery", &show_effect_gallery)) {
                ImGui::TextUnformatted("Each row uses a different Slang effect on the RGB quads.");
                ImGui::Separator();
                const float sz = 44.f * dpi_scale;
                const float gap = 6.f * dpi_scale;

                if (sepiaEffect.IsValid()) {
                    struct UBO { float uMix; float _pad[3]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(sepiaEffect, UBO{ s_sepiaMix, {} });
                    ImGui::SliderFloat("Sepia mix", &s_sepiaMix, 0.f, 1.f);
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, sepiaEffect);
                    if (row)
                        DrawEffectPreviewQuads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                    ImGui::Separator();
                }

                if (pixelateEffect.IsValid()) {
                    struct UBO { float uBlocks; float _pad[3]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(pixelateEffect, UBO{ s_pixelateBlocks, {} });
                    ImGui::SliderFloat("Pixelate cells", &s_pixelateBlocks, 2.f, 64.f);
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, pixelateEffect);
                    if (row)
                        DrawEffectPreviewQuads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                    ImGui::Separator();
                }

                if (chromaticEffect.IsValid()) {
                    struct UBO { float uStrength; float _pad[3]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(chromaticEffect, UBO{ s_chromaStrength, {} });
                    ImGui::SliderFloat("Chromatic UV shift", &s_chromaStrength, 0.f, 0.025f, "%.4f");
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, chromaticEffect);
                    if (row)
                        DrawEffectPreviewQuads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                    ImGui::Separator();
                }

                if (invertEffect.IsValid()) {
                    struct UBO { float uMix; float _pad[3]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(invertEffect, UBO{ s_invertMix, {} });
                    ImGui::SliderFloat("Invert mix", &s_invertMix, 0.f, 1.f);
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, invertEffect);
                    if (row)
                        DrawEffectPreviewQuads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                    ImGui::Separator();
                }

                if (scanlinesEffect.IsValid()) {
                    struct UBO { float uIntensity, uLines; float _pad[2]; };
                    IMGUI_EFFECT_UNIFORM_STRUCT(UBO);
                    effectSystem.SetEffectUniformStruct(scanlinesEffect, UBO{ s_scanlineIntensity, s_scanlineLines, {} });
                    ImGui::SliderFloat("Scanline darken", &s_scanlineIntensity, 0.f, 0.85f);
                    ImGui::SliderFloat("Scanline bands", &s_scanlineLines, 20.f, 400.f);
                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, scanlinesEffect);
                    if (row)
                        DrawEffectPreviewQuads(ImGui::GetWindowDrawList(), whiteTexImId, ImGui::GetCursorScreenPos(), sz, gap);
                }

                if (figmaFillEffect.IsValid()) {
                    ImGui::Separator();
                    ImGui::TextUnformatted("Figma fills (Linear, Radial, Angular, Diamond) — UV 0..1");
                    ImGui::Combo("Type", &s_figmaGtype, "Linear\0Radial\0Angular\0Diamond\0");
                    ImGui::Combo("Spread", &s_figmaSpread, "Pad (clamp)\0Repeat\0Reflect\0");
                    ImGui::SliderFloat("Dither (banding)", &s_figmaDither, 0.f, 1.f);
                    ImGui::SliderFloat("Frame rotate (deg)", &s_figmaFrameRotDeg, -180.f, 180.f);
                    ImGui::SliderFloat("Frame scale X", &s_figmaFrameSx, 0.2f, 3.f);
                    ImGui::SliderFloat("Frame scale Y", &s_figmaFrameSy, 0.2f, 3.f);
                    ImGui::SliderFloat("Frame translate X", &s_figmaFrameTx, -0.5f, 0.5f);
                    ImGui::SliderFloat("Frame translate Y", &s_figmaFrameTy, -0.5f, 0.5f);
                    if (figma_palette_16f_ok) {
                        ImGui::Checkbox("Palette RGBA16F (linear, less banding)", &s_figmaPalette16f);
                    } else {
                        ImGui::TextUnformatted("RGBA16F palette: GL rejected (using 8-bit sRGB texels).");
                        s_figmaPalette16f = false;
                    }
                    ImGui::SliderInt("Color stops", &s_figmaStopCount, 2, kFigmaMaxStops);

                    IMGUI_EFFECT_UNIFORM_STRUCT(FigmaUnifiedGradientUBO);
                    for (int si = 0; si < s_figmaStopCount; ++si) {
                        ImGui::PushID(si);
                        ImGui::ColorEdit4("Color", s_figmaC[si], ImGuiColorEditFlags_Float);
                        ImGui::DragFloat("Pos", &s_figmaS[si], 0.01f, 0.f, 1.f);
                        ImGui::PopID();
                    }
                    ImGui::SliderFloat("Blend fill over pixmap", &s_figmaReplace, 0.f, 1.f);

                    if (s_figmaGtype == 0) {
                        ImGui::TextUnformatted("Linear: handles A/B (like Figma endpoints, normalized UV).");
                        ImGui::SliderFloat("Handle A X", &s_figmaLinA[0], 0.f, 1.f);
                        ImGui::SliderFloat("Handle A Y", &s_figmaLinA[1], 0.f, 1.f);
                        ImGui::SliderFloat("Handle B X", &s_figmaLinB[0], 0.f, 1.f);
                        ImGui::SliderFloat("Handle B Y", &s_figmaLinB[1], 0.f, 1.f);
                    } else {
                        ImGui::TextUnformatted("Center / ellipse half-axes in UV (radial ellipse, angular pivot, diamond scale).");
                        ImGui::SliderFloat("Center X", &s_figmaCenter[0], 0.f, 1.f);
                        ImGui::SliderFloat("Center Y", &s_figmaCenter[1], 0.f, 1.f);
                        ImGui::SliderFloat("Ellipse X", &s_figmaEllipse[0], 0.02f, 1.5f);
                        ImGui::SliderFloat("Ellipse Y", &s_figmaEllipse[1], 0.02f, 1.5f);
                        if (s_figmaGtype == 2)
                            ImGui::SliderFloat("Rotate gradient start (deg)", &s_figmaAngleDeg, -180.f, 180.f);
                    }

                    effectSystem.SetEffectPaletteTexture(figmaFillEffect, (ImTextureID)(intptr_t)figma_palette_gl);
                    const bool use16fPal = figma_palette_16f_ok && s_figmaPalette16f;
                    RebuildFigmaPaletteTexture(figma_palette_gl, kFigmaPaletteTexWidth, s_figmaStopCount, s_figmaC,
                                               s_figmaS, use16fPal);

                    FigmaUnifiedGradientUBO gu{};
                    gu.linearAx = s_figmaLinA[0];
                    gu.linearAy = s_figmaLinA[1];
                    gu.linearBx = s_figmaLinB[0];
                    gu.linearBy = s_figmaLinB[1];
                    gu.centerX = s_figmaCenter[0];
                    gu.centerY = s_figmaCenter[1];
                    gu.ellipseX = s_figmaEllipse[0];
                    gu.ellipseY = s_figmaEllipse[1];
                    gu.gtype = static_cast<float>(s_figmaGtype);
                    gu.replaceMix = s_figmaReplace;
                    gu.angleStart = s_figmaAngleDeg * (3.14159265359f / 180.f);
                    gu.paletteWidth = static_cast<float>(kFigmaPaletteTexWidth);
                    gu.spreadMode = static_cast<float>(s_figmaSpread);
                    gu.ditherStrength = s_figmaDither;
                    FigmaBuildGradientAffine(s_figmaFrameRotDeg, s_figmaFrameSx, s_figmaFrameSy, s_figmaFrameTx,
                                            s_figmaFrameTy, &gu.gradM0);
                    gu.paletteLinear = use16fPal ? 1.f : 0.f;
                    gu._pad0 = gu._pad1 = gu._pad2 = 0.f;

                    effectSystem.SetEffectUniformStruct(figmaFillEffect, gu);

                    ImGuiRenderUX::EffectDrawRegionScope row(effectSystem, figmaFillEffect);
                    if (row) {
                        ImVec2 p0 = ImGui::GetCursorScreenPos();
                        float gw = 300.f * dpi_scale;
                        float gh = 140.f * dpi_scale;
                        ImGui::GetWindowDrawList()->AddImage(whiteTexImId, p0, ImVec2(p0.x + gw, p0.y + gh),
                                                             ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                        ImGui::Dummy(ImVec2(gw, gh));
                    }
                    ImGui::TextWrapped(
                        "Figma-like fills: linear / radial / angular / diamond in local UV, optional spread (pad|repeat|reflect), "
                        "UV frame affine (rotate/scale/translate), per-stop sRGB-linear interpolation, alpha in palette, dither, "
                        "optional RGBA16F palette. Not a full vector engine: no mesh warp, boolean fills, or plugin JSON import "
                        "(see docs/FIGMA_GRADIENT.md).");
                }
            }
            ImGui::End();
        }

        if (show_effect_debug)
            effectSystem.ShowDebugWindow(&show_effect_debug);

        {
            ImGuiRenderUX::EffectSubmitGuard submitGuard(effectSystem);
            (void)submitGuard;
        }

        DrUI::ToastAnchor anchor{layout.canvas.pos, layout.canvas.size};
        DrUI::DrawToasts(anchor);

        if (panels.status_bar) DrawStatusBar(dpi_scale);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(DrUI::Colors::BackgroundPrimary.x, DrUI::Colors::BackgroundPrimary.y,
                     DrUI::Colors::BackgroundPrimary.z, DrUI::Colors::BackgroundPrimary.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3Slang_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_context);
        }

        glfwSwapBuffers(window);
    }

    if (figma_palette_gl != 0)
        glDeleteTextures(1, &figma_palette_gl);
    builtinTextures.Destroy();
    ImGui_ImplOpenGL3Slang_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
