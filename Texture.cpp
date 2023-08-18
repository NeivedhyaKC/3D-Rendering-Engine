#include "Texture.h"
#include "Surface.h"
Texture::Texture(Graphics& gfx, const Surface& surface)
{
    INFOMAN(gfx);
    D3D11_TEXTURE2D_DESC t2Desc = {};
    t2Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    t2Desc.ArraySize = 1;
    t2Desc.MipLevels = 1;
    t2Desc.Usage = D3D11_USAGE_DEFAULT;
    t2Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    t2Desc.CPUAccessFlags = 0;
    t2Desc.MiscFlags = 0;
    t2Desc.Height = surface.GetHeight();
    t2Desc.Width = surface.GetWidth();
    t2Desc.SampleDesc.Count = 1;
    t2Desc.SampleDesc.Quality = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
    D3D11_SUBRESOURCE_DATA textureSd = {};
    textureSd.pSysMem = surface.GetBufferPtr();
    textureSd.SysMemPitch = surface.GetWidth() * sizeof(Surface::Color);
    GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&t2Desc, &textureSd, &pTexture));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = t2Desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &psrv));
}

void Texture::Bind(Graphics& gfx) noexcept
{
    GetDeviceContext(gfx)->PSSetShaderResources(0u, 1u, psrv.GetAddressOf());
}
