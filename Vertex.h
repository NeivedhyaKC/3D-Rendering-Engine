#pragma once
#include <vector>
#include "Graphics.h"
namespace DynamicVertex
{
    struct BGRAColor
    {
        unsigned char a;
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    class VertexLayout
    {
    public:
        enum ElementType
        {
            Position2D,
            Position3D,
            Texture2D,
            Normal,
            Float3Color,
            Float4Color,
            BGRAColor,
            Count
        };

        template<ElementType>
        struct Map;
        template<>
        struct Map<Position2D>
        {
            
            using SysType = DirectX::XMFLOAT2;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
            static constexpr const char* semantic = "Position";
        };
        template<> struct Map<Position3D>
        {
            using SysType = DirectX::XMFLOAT3;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            static constexpr const char* semantic = "Position";
        };
        template<> struct Map<Texture2D>
        {
            using SysType = DirectX::XMFLOAT2;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
            static constexpr const char* semantic = "Texcoord";
        };
        template<> struct Map<Normal>
        {
            using SysType = DirectX::XMFLOAT3;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            static constexpr const char* semantic = "Normal";
        };
        template<> struct Map<Float3Color>
        {
            using SysType = DirectX::XMFLOAT3;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            static constexpr const char* semantic = "Color";
        };
        template<> struct Map<Float4Color>
        {
            using SysType = DirectX::XMFLOAT4;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            static constexpr const char* semantic = "Color";
        };
        template<> struct Map<BGRAColor>
        {
            using SysType = DynamicVertex::BGRAColor;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            static constexpr const char* semantic = "Color";
        };
        class Element
        {
        public:
            Element(ElementType type, size_t offset) noexcept :
                type(type),
                offset(offset)
            {
            }
            ElementType GetType() const noexcept
            {
                return type;
            }
            size_t GetOffset() const noexcept
            {
                return offset;
            }
            size_t GetoffsetAfter() const noexcept
            {
                return offset + Size();
            }
            size_t Size() const noexcept
            {
                return SizeOf(type);
            }
            size_t SizeOf(ElementType type) const noexcept
            {
                switch (type)
                {
                case Position2D:
                    return sizeof(Map<Position2D>::SysType);
                case Position3D:
                    return sizeof(Map<Position3D>::SysType);
                case Texture2D:
                    return sizeof(Map<Texture2D>::SysType);
                case Normal:
                    return sizeof(Map<Normal>::SysType);
                case Float3Color:
                    return sizeof(Map<Float3Color>::SysType);
                case Float4Color:
                    return sizeof(Map<Float4Color>::SysType);
                case BGRAColor:
                    return sizeof(Map<BGRAColor>::SysType);
                }
            }
            D3D11_INPUT_ELEMENT_DESC GetDesc() const
            {
                switch (GetType())
                {
                case Position2D:
                    return GenerateDesc<Position2D>();
                case Position3D:
                    return GenerateDesc<Position3D>();
                case Texture2D:
                    return GenerateDesc<Texture2D>();
                case Normal:
                    return GenerateDesc<Normal>();
                case Float3Color:
                    return GenerateDesc<Float3Color>();
                case Float4Color:
                    return GenerateDesc<Float4Color>();
                case BGRAColor:
                    return GenerateDesc<BGRAColor>();
                }
                assert("Invalid element Type" && false);
                return { "INVALID",0,DXGI_FORMAT_UNKNOWN,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
            }
            template<ElementType type>
            constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc() const 
            {
                return { Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 };
            }
        private:
            ElementType type;
            size_t offset;
        };
    public:
        std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const
        {
            std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
            desc.reserve(GetElementCount());
            for (const Element& element : elements)
            {
                desc.push_back(element.GetDesc());
            }
            return desc;
        }
        const Element& ResolveByIndex(size_t i) const noexcept(!IS_DEBUG)
        {
            return elements[i];
        }
        template <ElementType type>
        const Element& Resolve() const noexcept(!IS_DEBUG)
        {
            for (auto& element : elements)
            {
                if (element.GetType() == type)
                {
                    return element;
                }
            }
            assert("Couldn't find element of specified type" && false);
            return elements.front();
        }
        VertexLayout& Append(ElementType type) noexcept(!IS_DEBUG)
        {
            elements.emplace_back(type, Size());
            return *this;
        }
        size_t GetElementCount() const noexcept
        {
            return elements.size();
        }
        size_t Size()const noexcept(!IS_DEBUG)
        {
            return elements.empty() ? 0u : elements.back().GetoffsetAfter();
        }
    private:
        std::vector<Element> elements;
    };

    class Vertex
    {
        friend class VertexBuffer;
    public:
        Vertex(char* pData, VertexLayout& layout) :
            pData(pData),
            layout(layout)
        {
        }
        template<VertexLayout::ElementType Type>
        auto& Attr() const
        {
            auto& element = layout.Resolve<Type>();
            auto pAttribute = pData + element.GetOffset();
            return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
        }
        template <VertexLayout::ElementType DestLayoutType, typename Src>
        void SetAttribute(char* pAttribute, Src&& val)
        {
            using Dest = typename  VertexLayout::Map<DestLayoutType>::SysType;
            if constexpr(std::is_assignable<Dest, Src>::value)
            {
                *reinterpret_cast<Dest*>(pAttribute) = val;
            }
            else
            {
                assert("source ans dest type are not assignable" && false);
            }
        }
        template <typename First>
        void SetAttributByIndex(size_t i, First&& first)
        {
            auto& element = layout.ResolveByIndex(i);
            char* pAttribute = pData + element.GetOffset();
            switch (element.GetType())
            {
            case VertexLayout::ElementType::Position3D:
                SetAttribute<VertexLayout::ElementType::Position3D>(pAttribute, std::forward<First>(first));
                break;
            case VertexLayout::ElementType::Position2D:
                SetAttribute<VertexLayout::ElementType::Position2D>(pAttribute, std::forward<First>(first));
                break;
            case VertexLayout::ElementType::Normal:
                SetAttribute<VertexLayout::ElementType::Normal>(pAttribute, std::forward<First>(first));
                break;
            case VertexLayout::ElementType::Texture2D:
                SetAttribute<VertexLayout::ElementType::Texture2D>(pAttribute, std::forward<First>(first));
                break;
            case VertexLayout::ElementType::Float3Color:
                SetAttribute<VertexLayout::ElementType::Float3Color>(pAttribute, std::forward<First>(first));
                break;
            case VertexLayout::ElementType::Float4Color:
                SetAttribute<VertexLayout::ElementType::Float4Color>(pAttribute, std::forward<First>(first));
                break;
            case VertexLayout::ElementType::BGRAColor:
                SetAttribute<VertexLayout::ElementType::BGRAColor>(pAttribute, std::forward<First>(first));
                break;
            }
        }
        template <typename First, typename ...Rest>
        void SetAttributByIndex(size_t i, First&& first, Rest&&... rest)
        {
            SetAttributByIndex(i, std::forward<First>(first));
            SetAttributByIndex(i + 1, std::forward<Rest>(rest)...);
        }
    private:
        char* pData;
        VertexLayout& layout;
    };
    class VertexBuffer
    {
    public:
        VertexBuffer(VertexLayout vertexLayout) :
            vertexLayout(std::move(vertexLayout))
        {
        }
        template<typename ...Params>
        void Emplace_Back(Params&&... params)
        {
            buffer.resize(buffer.size() + vertexLayout.Size());
            Back().SetAttributByIndex(0u, std::forward<Params>(params)...);
        }
        const char* GetData() const
        {
            return buffer.data();
        }
        const VertexLayout& GetLayout() const
        {
            return vertexLayout;
        }
        Vertex Front()
        {
            assert(buffer.size() != 0);
            return Vertex{ buffer.data(),vertexLayout };
        }
        Vertex Back()
        {
            assert(buffer.size() != 0);
            return Vertex{ buffer.data() + buffer.size() - vertexLayout.Size(), vertexLayout };
        }
        Vertex operator[](size_t i)
        {
            assert(i < Size());
            return Vertex{ buffer.data() + vertexLayout.Size() * i,vertexLayout };
        }
        size_t Size() const 
        {
            return buffer.size() / vertexLayout.Size();
        }
        size_t SizeInBytes() const 
        {
            return buffer.size();
        }
    private:
        VertexLayout vertexLayout;
        std::vector<char> buffer;
    };
}


