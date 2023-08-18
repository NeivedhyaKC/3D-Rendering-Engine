#pragma once
#include "DrawableBase.h"
#include "assimp//Importer.hpp"
#include "assimp//postprocess.h"
#include "assimp/scene.h"
#include <string>
#include <optional>
class Mesh :public DrawableBase<Mesh>
{
public:
    Mesh(Graphics& gfx,std::vector<std::unique_ptr<Bindable>> bindables);
    virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
    virtual void Draw(Graphics& gfx, DirectX::FXMMATRIX accumalatedTransform) const;
private:
    mutable DirectX::XMFLOAT4X4 transform;
};
class Node
{
    friend class Model;
public:
    Node(const std::string& name, const DirectX::XMMATRIX& transform, std::vector<Mesh*> meshPtrs,int id);
    void Draw(Graphics& gfx, DirectX::FXMMATRIX accumalatedTransform);
    void ShowTree(Node*& pSelectedNode) const;
    const int& GetID();
private:
    void SetAppliedTransform(DirectX::XMMATRIX appliedTransform);
    void AddChild(std::unique_ptr<Node> pChild);
    const std::string name;
    std::vector<std::unique_ptr<Node>> childPtrs;
    std::vector<Mesh*> meshPtrs;
    DirectX::XMFLOAT4X4 transform;
    DirectX::XMFLOAT4X4 appliedTransform;
    const int id;
};
class Model
{
public:
    Model(Graphics& gfx, std::string filename);
    ~Model();
    void Draw(Graphics& gfx);
    void ShowWindow();
    std::unique_ptr<Node> ParseNode(const aiNode& node, int& id);
    std::unique_ptr<Mesh> ParseMesh(Graphics& gfx,const aiMesh& mesh,const aiMaterial*const* pMaterials);
private:
    std::unique_ptr<Node> pRoot = nullptr;
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::unique_ptr<class ModelWindow> pModelWindow = nullptr;
};

