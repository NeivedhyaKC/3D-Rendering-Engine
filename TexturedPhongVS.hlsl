cbuffer transforms
{
    matrix modelView;
    matrix modelViewProj;
}

struct VSOutput
{
    float3 camPos : Position;
    float3 n : Normal;
    float2 tc : TexCoord;
    float4 svpos : SV_Position;
};

VSOutput main(float3 pos : Position, float3 n : Normal,float2 tc:TexCoord)
{
    VSOutput vsOutput;
    vsOutput.camPos = mul(float4(pos, 1.0f), modelView);
    vsOutput.n = mul(n, (float3x3) modelView);
    vsOutput.tc = tc;
    vsOutput.svpos = mul(float4(pos, 1.0f), modelViewProj);
    return vsOutput;
}