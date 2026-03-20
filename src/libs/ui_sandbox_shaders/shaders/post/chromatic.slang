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
