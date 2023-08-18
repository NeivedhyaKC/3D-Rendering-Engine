#include "Melon.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformCbuff.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "Sphere.h"

Melon::Melon(Graphics& gfx, std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist,
    std::uniform_int_distribution<int>& longdist,
    std::uniform_int_distribution<int>& latdist) :
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
		auto pvs = std::make_unique<VertexShader>(gfx, L"IndexedColorVS.cso");
		auto pvsByteCode = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"IndexedColorPS.cso"));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsByteCode));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSConstantBuff
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} faceColors[8];
		};
		const PSConstantBuff cb2 =
		{
			{
				{ 1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f },
			}
		};
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSConstantBuff>>(gfx, cb2));
	}
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
	};
	auto itl = Sphere::MakeTesselated<Vertex>(latdist(rng), longdist(rng));
	itl.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.2f));
	AddBind(std::make_unique<VertexBuffer>(gfx, itl.vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, itl.indices));

	AddBind(std::make_unique<TransformCbuff>(gfx, *this));
}

void Melon::Update(float dt)
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Melon::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) * 
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
