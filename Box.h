#pragma once
#include "TestObject.h"
#include<random>

class Box : public TestObject<Box>
{
public:
	Box(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		std::uniform_real_distribution<float>& cdist
		);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	bool SpawnControlWindow(Graphics& gfx);
	
private:
	void SyncMaterial(Graphics& gfx);

	DirectX::XMFLOAT3X3 mt;
	struct MaterialColor
	{
		DirectX::XMFLOAT3 materialColor;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	};	
	MaterialColor mat;
};