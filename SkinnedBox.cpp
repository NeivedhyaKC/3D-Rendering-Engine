#include "SkinnedBox.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformCbuff.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "Sampler.h"
#include "Texture.h"
#include"Surface.h"
#include "Cube.h"

SkinnedBox::SkinnedBox(Graphics& gfx, std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist):
	TestObject(gfx,rng,adist,ddist,odist,rdist)
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
			struct
			{
				float u;
				float v;
			} tc;
		};

		auto itl = Cube::MakeIndependentTextured<Vertex>();
		itl.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, itl.vertices));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, itl.indices));

		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"TexturedPhongVS.cso");
		auto pByteCode = pVertexShader->GetBytecode();
		AddStaticBind(std::move(pVertexShader));
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturedPhongPS.cso"));

		std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0}
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pByteCode));
		AddStaticBind(std::make_unique<Sampler>(gfx));
		auto surface = Surface::FromFile("Images\\kappa50.png");
		AddStaticBind(std::make_unique<Texture>(gfx, surface));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		struct PSMaterialConstant
		{
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2];
		};
		PSMaterialConstant mat;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, mat, 1u));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuff>(gfx, *this));
}