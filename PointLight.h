#pragma once
#include "ConstantBuffer.h"
#include "SolidSphere.h"
class PointLight
{
public:
    PointLight(Graphics& gfx, float radius);
    void SpawnControlWindow();
    void Reset();
    void Draw(Graphics& gfx) noexcept;
    void Bind(Graphics& gfx, DirectX::FXMMATRIX view) noexcept;
    struct PointLightBuff
    {
        alignas(16) DirectX::XMFLOAT3 lightPos = { 0.0f,0.0f,0.0f };
        alignas(16) DirectX::XMFLOAT3 ambient = { 0.05f, 0.05f, 0.05f };
        alignas(16) DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
        float diffuseIntensity = 1.0f;
        float attConst = 1.0f;
        float attLin = 0.045f;
        float attQuad = 0.0075f;
    };
private:
    PixelConstantBuffer<PointLightBuff> pcBuff;
    SolidSphere solidSphere;
    PointLightBuff pointLightBuff;
};

