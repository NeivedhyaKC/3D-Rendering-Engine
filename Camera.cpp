#include "Camera.h"
#include "imgui.h"
#include "ChiliMath.h"
#include <algorithm>

Camera::Camera()
{
    Reset();
}

void Camera::Reset()
{
    pitch = 0.0f;
    yaw = 0.0f;
    pos = { 0.0f,8.0f, -6.0f };
}

DirectX::XMMATRIX Camera::GetMatrix()
{
    using namespace DirectX;
    const auto lookVector = DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));
    const auto camPos = DirectX::XMLoadFloat3(&pos);
    return DirectX::XMMatrixLookAtLH(camPos, camPos + lookVector, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Camera::SpawnControlWindow()
{
    /*if (ImGui::Begin("Camera Transform"))
    {
        ImGui::Text("Position");
        ImGui::SliderFloat("r", &r, -40.0f, 40.0f);
        ImGui::SliderAngle("theta");
        ImGui::SliderAngle("phi", &phi);
        ImGui::Text("Rotation");
        ImGui::SliderAngle("Pitch", &pitch);
        ImGui::SliderAngle("Yaw", &yaw);
        ImGui::SliderAngle("Roll", &roll);
        if (ImGui::Button("Reset"))
        {
            Reset();
        }
    }
    ImGui::End();*/
}

void Camera::Rotate(int dx, int dy)
{
    pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
    yaw = wrap_angle(yaw + dx * rotationSpeed);
}

void Camera::Translate(float x, float y, float z)
{
    DirectX::XMFLOAT3 translation;
    DirectX::XMStoreFloat3(&translation,DirectX::XMVector3Transform(DirectX::XMVectorSet(x * translationSpeed, y * translationSpeed, z * translationSpeed, 1.0f),
        DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0)));
    pos = {
        pos.x + translation.x,
        pos.y + translation.y,
        pos.z + translation.z,
    };
}
