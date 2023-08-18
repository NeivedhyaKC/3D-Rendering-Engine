cbuffer transforms
{
    matrix modelView;
    matrix modelViewProj;
}

struct VSOutput
{
    float3 camPos : Position;
    float3 n : Normal;
    float3 color : Color;
    float4 svpos : SV_Position;
};

VSOutput main(float3 pos : Position, float3 n : Normal, float3 color :Color)
{
    VSOutput vsOutput;
    vsOutput.camPos = mul(float4(pos, 1.0f), modelView);
    vsOutput.n = mul(n, (float3x3) modelView);
    vsOutput.svpos = mul(float4(pos, 1.0f), modelViewProj);
    vsOutput.color = color;
    return vsOutput;
}