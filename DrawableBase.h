#pragma once
#include "Drawable.h"
template<typename T>
class DrawableBase : public Drawable
{
protected:

    bool IsStaticInitialized();
    void AddStaticBind(std::unique_ptr<Bindable> bindable);
    void AddStaticIndexBuffer(std::unique_ptr<class IndexBuffer> bindable);
    void SetIndexFromStatic() noexcept(!IS_DEBUG);
private:

    virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const override;

    static std::vector<std::unique_ptr<Bindable>> staticBinds;
};
template<typename T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;

template<typename T>
inline bool DrawableBase<T>::IsStaticInitialized()
{
    return !staticBinds.empty();
}

template<typename T>
const std::vector<std::unique_ptr<Bindable>>& DrawableBase<T>::GetStaticBinds() const
{
    return staticBinds;
}

template<typename T>
void DrawableBase<T>::AddStaticBind(std::unique_ptr<Bindable> bindable)
{
    assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(IndexBuffer) != typeid(*bindable));
    staticBinds.push_back(std::move(bindable));
}

template<typename T>
void DrawableBase<T>::AddStaticIndexBuffer(std::unique_ptr<class IndexBuffer> bindable)
{
    assert("*Must* use AddIndexBuffer to bind index buffer" && pIndexBuffer == nullptr);
    pIndexBuffer = bindable.get();
    staticBinds.push_back(std::move(bindable));
}
template<typename T>
inline void DrawableBase<T>::SetIndexFromStatic() noexcept(!IS_DEBUG)
{
    assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
    for (const auto& b : staticBinds)
    {
        if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
        {
            pIndexBuffer = p;
            return;
        }
    }
    assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
}