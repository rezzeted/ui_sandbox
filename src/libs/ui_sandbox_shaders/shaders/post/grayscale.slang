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
