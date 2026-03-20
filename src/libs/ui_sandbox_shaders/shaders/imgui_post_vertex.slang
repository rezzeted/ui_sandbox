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
