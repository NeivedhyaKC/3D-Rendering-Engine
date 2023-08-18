cbuffer cBuf
{
	matrix transform;
}

struct VSOutput
{
	float4 color : Color;
	float4 pos : SV_POSITION;
};

VSOutput main( float3 pos : Position ,float4 color :Color) 
{
	VSOutput vsOutput;
	vsOutput.color = color;
	vsOutput.pos = mul(float4(pos,1.0f), transform);
	return vsOutput;
}