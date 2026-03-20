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
