#include "TransformCBuff.h"

std::unique_ptr<VertexConstantBuffer<TransformCbuff::Transforms>> TransformCbuff::vcbuf;

TransformCbuff::TransformCbuff(Graphics& gfx, const Drawable& parent):
    parent(parent)
{
	vcbuf = std::make_unique < VertexConstantBuffer<TransformCbuff::Transforms>>(gfx);
}
void TransformCbuff::Bind(Graphics& gfx) noexcept
{
	Transforms transforms;
	auto modelView = parent.GetTransformXM() * gfx.GetCamera();
	transforms.modelViewProj = DirectX::XMMatrixTranspose(modelView * gfx.GetProjection());
	transforms.modelView = DirectX::XMMatrixTranspose( modelView );
	vcbuf->Update(gfx,transforms);
	vcbuf->Bind(gfx);
}