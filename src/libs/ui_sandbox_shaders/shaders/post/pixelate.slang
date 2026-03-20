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
