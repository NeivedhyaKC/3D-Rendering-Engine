#include "Drawable.h"
#include"IndexBuffer.h"
void Drawable::Draw(Graphics& gfx) const
{
    for (auto& staticBind : GetStaticBinds())
    {
        staticBind->Bind(gfx);
    }
    for (auto& bind : binds)
    {
        bind->Bind(gfx);
    }
    gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bindable)
{
    assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(IndexBuffer) != typeid(*bindable));
    binds.push_back(std::move(bindable));
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> bindable)
{
    assert("*Must* use AddIndexBuffer to bind index buffer" && pIndexBuffer==nullptr);
    pIndexBuffer = bindable.get();
    binds.push_back(std::move(bindable));
}
