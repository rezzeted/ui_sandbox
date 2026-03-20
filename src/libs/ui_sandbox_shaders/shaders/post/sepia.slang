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
