#pragma once
#include "ConstantBuffer.h"
#include "Drawable.h"
#include <DirectXMath.h>

class TransformCbuff : public Bindable
{
public:
	TransformCbuff(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;
	struct Transforms
	{
		DirectX::XMMATRIX modelView;
		DirectX::XMMATRIX modelViewProj;
	};
private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> vcbuf;
	const Drawable& parent;
};
