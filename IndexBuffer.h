#pragma once
#include"Bindable.h"
class IndexBuffer : public Bindable
{
public:
    IndexBuffer(Graphics& gfx, std::vector<unsigned short> indices);
    void Bind(Graphics& gfx) noexcept override;
    unsigned int GetCount() const;
private:
    unsigned int count = 0;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
};

