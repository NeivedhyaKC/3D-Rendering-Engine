#pragma once
#include"Bindable.h"
class VertexShader : public Bindable
{
public:
    VertexShader(Graphics& gfx, std::wstring path);
    void Bind(Graphics& gfx) noexcept override;
    ID3DBlob* GetBytecode();
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
    Microsoft::WRL::ComPtr <ID3DBlob> pBytecodeBlob = nullptr;
};

