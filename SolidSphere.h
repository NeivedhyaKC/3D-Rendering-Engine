#pragma once
#include "DrawableBase.h"
class SolidSphere : public DrawableBase<SolidSphere>
{
public:
    void SetPos(DirectX::XMFLOAT3 position);
    SolidSphere(Graphics& gfx, float radius = 0.5f);
    virtual void Update(float dt) override {}
    virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
    DirectX::XMFLOAT3 pos;
};

