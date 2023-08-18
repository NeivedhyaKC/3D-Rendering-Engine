#include "Pyramid.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformCbuff.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "Cone.h"

Pyramid::Pyramid(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int> tdist) :
	TestObject(gfx,rng,adist,ddist,odist,rdist)
{
	if (!IsStaticInitialized())
	{

		auto pvs = std::make_unique<VertexShader>(gfx, L"BlendedPhongVS.cso");
		auto pvsByteCode = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"BlendedPhongPS.cso"));
		
		std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsByteCode));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		struct PSMaterialConstant
		{
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float pdding[2];
		};
		PSMaterialConstant mat;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, mat, 1u));
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
		struct
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} color;
	};
	int tessalation = tdist(rng);
	auto itl = Cone::MakeTesselatedIndependentFaces<Vertex>(tessalation);
	itl.SetNormalsIndependentFlat();
	for (Vertex& v : itl.vertices)
	{
		v.color = { (unsigned char)10,(unsigned char)10,(unsigned char)255 };
	}
	for (int i = 0; i < tessalation; i++)
	{
		itl.vertices[i * 3].color = { (unsigned char)255,(unsigned char)10,(unsigned char)10 };
	}		
	itl.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 0.7f));
	AddBind(std::make_unique<VertexBuffer>(gfx, itl.vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, itl.indices));
	AddBind(std::make_unique<TransformCbuff>(gfx, *this));
}