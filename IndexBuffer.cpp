#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Graphics& gfx, std::vector<unsigned short> indices):
    count(indices.size())
{
    INFOMAN(gfx);
    D3D11_BUFFER_DESC indexDesc = { 0 };
    indexDesc.Usage = D3D11_USAGE_DEFAULT;
    indexDesc.ByteWidth = UINT(count * sizeof(unsigned short));
    indexDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indexDesc.CPUAccessFlags = 0u;
    indexDesc.MiscFlags = 0u;
    indexDesc.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA indexSubData = { 0 };
    indexSubData.pSysMem = indices.data();
    GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&indexDesc, &indexSubData, &pIndexBuffer));
}

void IndexBuffer::Bind(Graphics& gfx) noexcept
{
    GetDeviceContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

unsigned int IndexBuffer::GetCount() const
{
    return count;
}
