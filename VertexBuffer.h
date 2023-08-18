#pragma once
#include "Vertex.h"
#include"Bindable.h"
class VertexBuffer : public Bindable
{
public:
    template<typename T>
    VertexBuffer(Graphics& gfx, std::vector<T> vertices):
        stride(sizeof(T))
    {
        INFOMAN(gfx);

        D3D11_BUFFER_DESC Desc = {};
        Desc.Usage = D3D11_USAGE_DEFAULT;
        Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        Desc.CPUAccessFlags = 0u;
        Desc.MiscFlags = 0u;
        Desc.ByteWidth = UINT(sizeof(T) * vertices.size());
        Desc.StructureByteStride = sizeof(T);
        D3D11_SUBRESOURCE_DATA subData = { 0 };

        subData.pSysMem = vertices.data();
        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&Desc, &subData, &pVertexBuffer));
    }
    VertexBuffer(Graphics& gfx, const DynamicVertex::VertexBuffer& vbuf)
        :
        stride((UINT)vbuf.GetLayout().Size())
    {
        INFOMAN(gfx);

        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.CPUAccessFlags = 0u;
        bd.MiscFlags = 0u;
        bd.ByteWidth = UINT(vbuf.SizeInBytes());
        bd.StructureByteStride = stride;
        D3D11_SUBRESOURCE_DATA sd = {};
        sd.pSysMem = vbuf.GetData();
        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
    }
    void Bind(Graphics& gfx) noexcept override;
private:
    unsigned int stride = 0;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
};

