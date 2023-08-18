#pragma once
#include <DirectXMath.h>
class Camera
{
public:
    Camera();
    void Reset();
    DirectX::XMMATRIX GetMatrix();
    void SpawnControlWindow();
    void Rotate(int dx, int dy);
    void Translate(float x, float y, float z);
private:
    const float rotationSpeed = 0.004f;
    const float translationSpeed = 0.2f;
    DirectX::XMFLOAT3 pos;
    float pitch = 0.0f;
    float yaw = 0.0f;
};                                                                                    

