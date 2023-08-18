#include "PointLight.h"
#include "imgui.h"

PointLight::PointLight(Graphics& gfx,float radius) :
    pcBuff(gfx),
    solidSphere(gfx,radius)
{
    Reset();
}

void PointLight::SpawnControlWindow()
{
    if (ImGui::Begin("Light Properties"))
    {
        ImGui::Text("Position");
        ImGui::SliderFloat("x", &pointLightBuff.lightPos.x,-60.0f,60.0f);
        ImGui::SliderFloat("y", &pointLightBuff.lightPos.y,-60.0f,60.0f);
        ImGui::SliderFloat("z", &pointLightBuff.lightPos.z,-60.0f,60.0f);
        ImGui::Text("Intensity/Color");
        ImGui::SliderFloat("Intensity", &pointLightBuff.diffuseIntensity,0.01f,2.0f);
        ImGui::ColorEdit3("Diffuse", &pointLightBuff.diffuseColor.x);
        ImGui::ColorEdit3("Ambient", &pointLightBuff.ambient.x);
        ImGui::Text("Falloff");
        ImGui::SliderFloat("Constant", &pointLightBuff.attConst, 0.05f, 10.0f);
        ImGui::SliderFloat("Linear", &pointLightBuff.attLin, 0.0001f, 4.0f);
        ImGui::SliderFloat("Quadratic", &pointLightBuff.attQuad, 0.0000001f, 10.0f);
        if (ImGui::Button("Reset"))
        {
            Reset();
        }
    }
    ImGui::End();
}

void PointLight::Reset()
{ 
    pointLightBuff.lightPos = { 5.0f,8.0f,-4.0f };
    pointLightBuff.ambient = { 0.05f, 0.05f, 0.05f };
    pointLightBuff.diffuseColor = { 1.0f, 1.0f, 1.0f };
    pointLightBuff.diffuseIntensity = 1.0f;
    pointLightBuff.attConst = 1.0f;
    pointLightBuff.attLin = 0.045f;
    pointLightBuff.attQuad = 0.0075f;
}

void PointLight::Draw(Graphics& gfx) noexcept
{
    solidSphere.SetPos(pointLightBuff.lightPos);
    solidSphere.Draw(gfx);
}

void PointLight::Bind(Graphics& gfx,DirectX::FXMMATRIX view) noexcept
{
    auto dataCopy = pointLightBuff;
    auto pos = DirectX::XMLoadFloat3(&pointLightBuff.lightPos);
    DirectX::XMStoreFloat3(&dataCopy.lightPos, DirectX::XMVector3Transform(pos, view));
    pcBuff.Update(gfx, dataCopy);
    pcBuff.Bind(gfx);
}
