#pragma once
#include "Window.h"
#include"ChilliTimer.h"
#include"Box.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include<set>

class App
{
public:
    App();
    int Go();
private:
    void DoFrame();

    int x = 0; int y = 0;
    bool showDemoWindow = true;
    float speedFactor = 1.0f;
    ImGuiManager imGuiManager;
    ChilliTimer m_timer;
    Window m_window;
    Camera camera;
    PointLight pointLight;
    Model nano;
    
};

