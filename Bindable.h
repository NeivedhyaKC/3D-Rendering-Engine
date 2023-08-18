#pragma once
#include<d3d11.h>
#include"Graphics.h"
class Bindable
{
public:
    virtual void Bind(Graphics& gfx) noexcept = 0;
    Bindable() = default;
    virtual ~Bindable() = default;
protected:
    static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
    static ID3D11DeviceContext* GetDeviceContext(Graphics& gfx) noexcept;
    static DXGIInfoManager& GetInfoManager(Graphics& gfx) noexcept;
};

