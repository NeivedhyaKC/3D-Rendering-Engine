#include "Bindable.h"

ID3D11Device* Bindable::GetDevice(Graphics& gfx) noexcept
{
    return gfx.pDevice.Get();
}

ID3D11DeviceContext* Bindable::GetDeviceContext(Graphics& gfx) noexcept
{
    return gfx.pDeviceContext.Get();
}

DXGIInfoManager& Bindable::GetInfoManager(Graphics& gfx) noexcept
{
    return gfx.infoManager;
}
