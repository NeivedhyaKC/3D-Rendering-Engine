#include "AssimpTest.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformCbuff.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "assimp//Importer.hpp"
#include "assimp//postprocess.h"
#include "assimp/scene.h"
#include "Vertex.h"

AssimpTest::AssimpTest(Graphics& gfx, std::mt19937& rng,
    std::uniform_real_distribution<float>& adist,
    std::uniform_real_distribution<float>& ddist,
    std::uniform_real_distribution<float>& odist,
    std::uniform_real_distribution<float>& rdist,
    float scale):
    TestObject(gfx,rng,adist,ddist,odist,rdist)
{
    if (!IsStaticInitialized())
    {
        using DynamicVertex::VertexLayout;
        VertexLayout vertexLayout;
        vertexLayout.Append(VertexLayout::ElementType::Position3D)
            .Append(VertexLayout::ElementType::Normal);
        DynamicVertex::VertexBuffer vertexBuffer(std::move(vertexLayout));
        Assimp::Importer imp;
        auto pModel = imp.ReadFile("Models\\suzanne.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
        auto pMesh = pModel->mMeshes[0];

        for (int i = 0; i < pMesh->mNumVertices; i++)
        {
            vertexBuffer.Emplace_Back(DirectX::XMFLOAT3(pMesh->mVertices[i].x * scale, pMesh->mVertices[i].y * scale, pMesh->mVertices[i].z * scale),
                *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i]));
        }

        std::vector<unsigned short> indices;
        indices.reserve(pMesh->mNumFaces * 3);
        for (int i = 0; i < pMesh->mNumFaces; i++)
        {
            auto& face = pMesh->mFaces[i];
            assert(face.mNumIndices == 3);
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertexBuffer));
        AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

        AddStaticBind(std::make_unique<Topology>(gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        auto pVertexShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
        auto pBytecode = pVertexShader->GetBytecode();
        AddStaticBind(std::move(pVertexShader));

        AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

        AddStaticBind(std::make_unique<InputLayout>(gfx, vertexBuffer.GetLayout().GetD3DLayout(), pBytecode));

        struct PSMaterialConstant
        {
            alignas(16) DirectX::XMFLOAT3 materialColor = {
                0.0f,0.6f,0.0f
            };
            float specularIntensity = 0.6f;
            float specularPower = 30.0f;
        };
        PSMaterialConstant mat;
        AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, mat,1u));
    }
    else
    {
        SetIndexFromStatic();
    }

    AddBind(std::make_unique<TransformCbuff>(gfx, *this));
}
