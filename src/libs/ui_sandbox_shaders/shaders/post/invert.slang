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
