#include "VertexShader.h"
#include<d3dcompiler.h>
#pragma comment(lib,"D3DCompiler.lib")


VertexShader::VertexShader(Graphics& gfx, std::wstring path)
{
    INFOMAN(gfx);
    GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBytecodeBlob));
    GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, &pVertexShader));
}

void VertexShader::Bind(Graphics& gfx) noexcept
{
    GetDeviceContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0);
}

ID3DBlob* VertexShader::GetBytecode()
{
    return pBytecodeBlob.Get();
}
