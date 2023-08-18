#include "Mesh.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "Vertex.h"
#include "Topology.h"
#include "TransformCBuff.h"
#include "imgui.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"
#include <unordered_map>

Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindables)
{
    if (!IsStaticInitialized())
    {
        AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    }
    for (auto& pBindable : bindables)
    {
        if (const auto& pIndex = dynamic_cast<IndexBuffer*>(pBindable.get()))
        {
            AddIndexBuffer(std::unique_ptr<IndexBuffer>(pIndex));
            pBindable.release();
        }
        else
        {
            AddBind(std::move(pBindable));
        }
    }
    AddBind(std::make_unique<TransformCbuff>(gfx, *this));
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    return DirectX::XMLoadFloat4x4(&this->transform);
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumalatedTransform) const
{
    DirectX::XMStoreFloat4x4(&this->transform, accumalatedTransform);
    Drawable::Draw(gfx);
}

Node::Node(const std::string& name, const DirectX::XMMATRIX& transform_in, std::vector<Mesh*> meshPtrs,int id_in):
    meshPtrs(std::move(meshPtrs)),
    name(name),
    id(id_in)
{
    DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&transform, transform_in);
}

void Node::SetAppliedTransform(DirectX::XMMATRIX appliedTransform)
{
    DirectX::XMStoreFloat4x4(&this->appliedTransform, appliedTransform);
}

void Node::AddChild(std::unique_ptr<Node> pChild)
{
    assert(pChild);
    childPtrs.push_back(std::move(pChild));
}

void Node::ShowTree(Node*& pSelectedNode) const 
{
    const auto& selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetID();
    ImGuiTreeNodeFlags flags = (ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow) |
        ((id == selectedId) ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected : 0) |
        (childPtrs.empty() ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf : 0);

    const bool expanded = ImGui::TreeNodeEx((void*)(intptr_t)id, flags, name.c_str());
    if (ImGui::IsItemClicked())
    {
        pSelectedNode = const_cast<Node*>(this);
    }
    if (expanded)
    {
        for (const auto& pChild : childPtrs)
        {
            pChild->ShowTree(pSelectedNode);
        }
        ImGui::TreePop();
    }
}

const int& Node::GetID()
{
    return id;
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumalatedTransform)
{
    auto built = DirectX::XMLoadFloat4x4(&appliedTransform) * DirectX::XMLoadFloat4x4(&transform) * accumalatedTransform;
    for (const auto& pMesh : meshPtrs)
    {
        pMesh->Draw(gfx,built);
    }
    for (const auto& pNode : childPtrs)
    {
        pNode->Draw(gfx,built);
    }
}

class ModelWindow
{
    friend class Model;
    struct TransformParameters
    {
        float roll = 0;
        float pitch = 0;
        float yaw = 0;
        float x = 0;
        float y = 0;
        float z = 0;
    };
    std::unordered_map<int, TransformParameters> transforms;
    Node* pSelectedNode = nullptr;
public:
    DirectX::XMMATRIX GetTranform()
    {
        const auto& transform = transforms.at(pSelectedNode->GetID());
        return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) * 
            DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
    }
    Node* GetSelectedNode() const
    {
        return pSelectedNode;
    }
    void SpawnModelWindow(const Node& root)
    {
        if (ImGui::Begin("Model Window"))
        {
            ImGui::Columns(2, nullptr, true);
            root.ShowTree(pSelectedNode);

            ImGui::NextColumn();
            if (pSelectedNode != nullptr)
            {
                auto& transform = transforms[pSelectedNode->GetID()];
                ImGui::Text("Position");
                ImGui::SliderFloat("X", &transform.x, -40, 40);
                ImGui::SliderFloat("Y", &transform.y, -40, 40);
                ImGui::SliderFloat("Z", &transform.z, -40, 40);
                ImGui::Text("Orientation");
                ImGui::SliderAngle("Pitch", &transform.pitch, -180, 180);
                ImGui::SliderAngle("Yaw", &transform.yaw, -180, 180);
                ImGui::SliderAngle("Roll", &transform.roll, -180, 180);
            }
            
        }
        ImGui::End();
    }
};

Model::Model(Graphics& gfx, std::string filename):
    pModelWindow(std::make_unique<ModelWindow>())
{
    Assimp::Importer imp;
    auto pScene = imp.ReadFile(filename.c_str(), aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals);
    meshes.reserve(pScene->mNumMeshes);
    
    for (int i = 0; i < pScene->mNumMeshes; i++)
    {
        meshes.push_back(ParseMesh(gfx,*pScene->mMeshes[i],pScene->mMaterials));
    }
    int id = 0;
    pRoot = ParseNode(*pScene->mRootNode,id);
}

Model::~Model()
{
}

void Model::Draw(Graphics& gfx)
{
    if (auto node = pModelWindow->GetSelectedNode())
    {
        node->SetAppliedTransform(pModelWindow->GetTranform());
    }
    pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow()
{
    pModelWindow->SpawnModelWindow(*pRoot);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial*const* pMaterials)
{
    using namespace std::literals;
    using DynamicVertex::VertexLayout;
    VertexLayout vertexLayout;
    vertexLayout.Append(DynamicVertex::VertexLayout::ElementType::Position3D);
    vertexLayout.Append(DynamicVertex::VertexLayout::ElementType::Normal);
    vertexLayout.Append(DynamicVertex::VertexLayout::ElementType::Texture2D);
    DynamicVertex::VertexBuffer vertexBuffer(std::move(vertexLayout));
    for (size_t i = 0; i < mesh.mNumVertices; i++)
    {
        vertexBuffer.Emplace_Back(*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
            *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
            *reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
    }
    assert(mesh.mFaces->mNumIndices == 3 && " number of indices of each face is not equal to three");
    std::vector<unsigned short> indices;
    indices.reserve(mesh.mNumFaces * 3);
    for (int i = 0; i < mesh.mNumFaces; i++)
    {
        const auto& face = mesh.mFaces[i];
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
    std::vector<std::unique_ptr<Bindable>> bindables;
    bindables.push_back(std::make_unique<VertexBuffer>(gfx, vertexBuffer));
    bindables.push_back(std::make_unique<IndexBuffer>(gfx, indices));
    auto pVertexShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
    auto pBytecode = pVertexShader->GetBytecode();
    bindables.push_back(std::move(pVertexShader));
    bindables.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));
    bindables.push_back(std::make_unique<InputLayout>(gfx, vertexBuffer.GetLayout().GetD3DLayout(), pBytecode));
    
    if (mesh.mMaterialIndex >= 0)
    {
        auto& material = *pMaterials[mesh.mMaterialIndex];
        aiString fileName;
        material.GetTexture(aiTextureType_DIFFUSE, 0, &fileName);
        bindables.push_back(std::make_unique<Texture>(gfx, Surface::FromFile("Models\\nano_textured\\"s + fileName.C_Str())));
        bindables.push_back(std::make_unique<Sampler>(gfx));
    }
    struct PSMaterialConstant
    {
        float specularIntensity = 0.6f;
        float specularPower = 30.0f;
        float padding[2];
    };
    PSMaterialConstant mat;
    bindables.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, mat, 1u));
    return std::make_unique<Mesh>(gfx, std::move(bindables));
}

std::unique_ptr<Node> Model::ParseNode(const aiNode& node, int& id)
{
    const auto& transform = DirectX::XMMatrixTranspose( DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)));

    std::vector<Mesh*> currentMeshptrs;
    currentMeshptrs.reserve(node.mNumMeshes);
    for (int i = 0; i < node.mNumMeshes; i++)
    {
        currentMeshptrs.push_back(meshes.at(node.mMeshes[i]).get());
    }
    auto pNode = std::make_unique<Node>(node.mName.C_Str(), transform, std::move(currentMeshptrs),id++);
    for (int i = 0; i < node.mNumChildren; i++)
    {
        pNode->AddChild(ParseNode(*node.mChildren[i],id));
    }
    return pNode;
}

