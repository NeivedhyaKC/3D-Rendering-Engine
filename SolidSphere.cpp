#include "SolidSphere.h"
#include "Sphere.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformCbuff.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

void SolidSphere::SetPos(DirectX::XMFLOAT3 position)
{
    this->pos = position;
}

SolidSphere::SolidSphere(Graphics& gfx, float radius) :
	pos()
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};
		auto itl = Sphere::Make<Vertex>();
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, itl.vertices));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, itl.indices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"SolidColorVS.cso");
		auto pvsByteCode = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"SolidColorPS.cso"));

		std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color;
			float padding;
		};
		PSColorConstant col = { {1.0f,1.0f,1.0f} };
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(gfx, col));
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsByteCode));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuff>(gfx, *this));
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);;
}
