#pragma once
#include"Bindable.h"
template<typename T>
class ConstantBuffer :public Bindable
{
public:
    ConstantBuffer(Graphics& gfx,const T& consts,unsigned int startSlot = 0u)
    {
        this->startSlot = startSlot;
        INFOMAN(gfx);
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags = 0u;
        cbDesc.ByteWidth = sizeof(consts);
        cbDesc.StructureByteStride = 0u;
        D3D11_SUBRESOURCE_DATA cbSubData = { 0 };
        cbSubData.pSysMem = &consts;
        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbDesc, &cbSubData, &pCBuffer));
    }
    ConstantBuffer(Graphics& gfx)
    {
        INFOMAN(gfx);
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags = 0u;
        cbDesc.ByteWidth = sizeof(T);
        cbDesc.StructureByteStride = 0u;
        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbDesc, nullptr, &pCBuffer));
    }

    void Update(Graphics& gfx, const T& consts)
    {
        INFOMAN(gfx);
        D3D11_MAPPED_SUBRESOURCE msr;
        GetDeviceContext(gfx)->Map(pCBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
        memcpy(msr.pData, &consts, sizeof(consts));
        GetDeviceContext(gfx)->Unmap(pCBuffer.Get(), 0u);
    }
protected:
    Microsoft::WRL::ComPtr<ID3D11Buffer> pCBuffer = nullptr;
    unsigned int startSlot = 0u;
};

template<typename T>
class VertexConstantBuffer : public ConstantBuffer<T>
{ 
public:
    using ConstantBuffer<T>::ConstantBuffer;
    using Bindable::GetDeviceContext;
    using ConstantBuffer<T>::pCBuffer;
    using ConstantBuffer<T>::startSlot;
    void Bind(Graphics& gfx) noexcept override
    {
        GetDeviceContext(gfx)->VSSetConstantBuffers(startSlot, 1u, pCBuffer.GetAddressOf());
    }
};
template<typename T>
class PixelConstantBuffer : public ConstantBuffer<T>
{
    using ConstantBuffer<T>::ConstantBuffer;
    using Bindable::GetDeviceContext;
    using ConstantBuffer<T>::pCBuffer;
    using ConstantBuffer<T>::startSlot;
public:
    void Bind(Graphics& gfx) noexcept override
    {
        GetDeviceContext(gfx)->PSSetConstantBuffers(startSlot, 1u, pCBuffer.GetAddressOf());
    }
};