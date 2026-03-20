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
