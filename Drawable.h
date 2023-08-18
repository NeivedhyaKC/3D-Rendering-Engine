#pragma once
#include<memory>
#include"Graphics.h"
#include<DirectXMath.h>
#include"Bindable.h"

class Drawable
{
public:
    Drawable() = default;
    Drawable(Drawable& drawable) = delete;
    virtual void Update(float dt) {}
    virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
    virtual void Draw(Graphics& gfx) const;
    ~Drawable() = default;
private:
    std::vector<std::unique_ptr<Bindable>> binds;
protected:
    void AddBind(std::unique_ptr<Bindable> bindable);
    void AddIndexBuffer(std::unique_ptr<class IndexBuffer> bindable);
    template<typename T>
    T* QueryBindable();

    virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const = 0;

    const IndexBuffer* pIndexBuffer = nullptr;
};

template<typename T>
inline T* Drawable::QueryBindable()
{
    for (auto& bind : binds)
    {
        if (auto ptr = dynamic_cast<T*>(bind.get()))
        {
            return ptr;
        }
    }
    assert("requesting for bind which doesnt exist in binds" && false);
    return nullptr;
}
