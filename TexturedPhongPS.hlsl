cbuffer LightPos
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
}
cbuffer material
{
    float specularIntensity;
    float specularPower;
};
Texture2D tex;
SamplerState samplr;

float4 main(float3 worldPos : Position, float3 n : Normal,float2 tc : TexCoord) : SV_TARGET
{
    float3 vToL = lightPos - worldPos;
    float distToL = length(vToL);
    float3 dirToL = vToL / distToL;
    float att = 1.0f / (attConst + attLin * distToL + attQuad * distToL * distToL);
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    // reflected light vector
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	// final color
    return float4(saturate(diffuse + ambient + specular), 1.0f) * tex.Sample(samplr, tc);
}