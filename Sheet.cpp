#include "Sheet.h"
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
#include "Plane.h"
#include"Surface.h"

Sheet::Sheet(Graphics& gfx, std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist):
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			struct
			{
				float u;
				float v;
			} tex;
		};

		auto itl = Plane::Make<Vertex>();
		itl.vertices[0].tex = { 0.0f,0.0f };
		itl.vertices[1].tex = { 0.0f,1.0f };
		itl.vertices[2].tex = { 1.0f,0.0f };
		itl.vertices[3].tex = { 1.0f,1.0f };

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, itl.vertices));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, itl.indices));
		
		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"TextureVS.cso");
		auto pByteCode = pVertexShader->GetBytecode();
		AddStaticBind(std::move(pVertexShader));
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturePS.cso"));

		std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pByteCode));
		AddStaticBind(std::make_unique<Sampler>(gfx));
		auto surface = Surface::FromFile("Images\\kappa50.png");
		AddStaticBind(std::make_unique<Texture>(gfx, surface));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuff>(gfx, *this));
}

void Sheet::Update(float dt)
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Sheet::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
