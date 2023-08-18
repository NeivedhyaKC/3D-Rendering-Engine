#include"Box.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformCbuff.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "Cube.h"
#include "imgui.h"

Box::Box(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	std::uniform_real_distribution<float>& cdist)
	:
	TestObject(gfx,rng,adist,ddist,odist,rdist)
{
	if (!IsStaticInitialized())
	{
		
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};
		IndexedTriangleList<Vertex> itl=  Cube::MakeIndependent<Vertex>();
		itl.SetNormalsIndependentFlat();
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, itl.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, itl.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		

	}
	else
	{
		SetIndexFromStatic();
	}
	
	mat.materialColor = { cdist(rng),cdist(rng) ,cdist(rng) };
	AddBind(std::make_unique <PixelConstantBuffer<MaterialColor>>(gfx, mat, 1u));
	AddBind(std::make_unique<TransformCbuff>(gfx, *this));

	DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3(&mt) * TestObject::GetTransformXM();
}

bool Box::SpawnControlWindow(Graphics& gfx)
{
	bool open = true;
	bool dirty = false;
	if (ImGui::Begin("Box Window",&open))
	{
		const bool mc = ImGui::ColorEdit3("Material Color", &mat.materialColor.x);
		const bool si = ImGui::SliderFloat("Specular Intensity", &mat.specularIntensity, 0.0f, 1.0f);
		const bool sp = ImGui::SliderFloat("Specular power", &mat.specularPower, 0.0f, 50.0);
		dirty = mc || si || sp;

        ImGui::Text("Position");
		ImGui::SliderFloat("r", &r, -20.0f, 20.0f);
		ImGui::SliderAngle("theta", &theta, -360.0f,360.0f);
		ImGui::SliderAngle("phi", &phi, -360.0f,360.0f);
		ImGui::SliderAngle("chi", &chi, -360.0f, 360.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("pitch", &pitch, -360.0f, 360.0f);
		ImGui::SliderAngle("yaw", &yaw, -360.0f, 360.0f);
		ImGui::SliderAngle("roll", &roll, -360.0f, 360.0f);
	}
	ImGui::End();
	if (dirty)
	{
		SyncMaterial(gfx);
	}
	return open;
}

void Box::SyncMaterial(Graphics& gfx)
{
	auto bindablePtr = QueryBindable<PixelConstantBuffer<MaterialColor>>();
	bindablePtr->Update(gfx, mat);
}
