#pragma once
#include "Bindable.h"

class Texture : public Bindable
{
public:
    Texture(Graphics& gfx, const class Surface& surface);
    virtual void Bind(Graphics& gfx) noexcept override;
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> psrv;
};

