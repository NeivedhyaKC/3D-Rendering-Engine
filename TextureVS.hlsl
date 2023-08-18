cbuffer cBuf
{
	matrix transform;
};

struct VSOutput
{
	float2 tc :TexCoord;
	float4 pos : SV_POSITION;
};

VSOutput main( float3 pos : Position, float2 tc :TexCoord )
{
	VSOutput vsOutput;
	vsOutput.pos = mul(float4(pos,1.0f), transform);
	vsOutput.tc = tc;
	return vsOutput;
}