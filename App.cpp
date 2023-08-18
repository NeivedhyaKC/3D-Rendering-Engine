#include "App.h"
#include<sstream>
#include<memory>
#include"Box.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Sheet.h"
#include "GDIPlusManager.h"
#include "SkinnedBox.h"
#include"imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Cylinder.h"
#include "AssimpTest.h"

GDIPlusManager gdiPlusManager;
App::App():
    m_window(1280, 720, L"Hello World"),
    pointLight(m_window.GetGraphics(),0.5f),
    nano(m_window.GetGraphics(), "Models\\nano_textured\\nanosuit.obj")
{
    m_window.GetGraphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 720.0f / 1280.0f, 0.5f, 40.0f));
    m_window.GetGraphics().SetCamera(DirectX::XMMATRIX());
    m_window.DisableCursor();
    m_window.m_mouse.EnableRaw();
}

int App::Go()
{
    MSG msg = { 0 };
    BOOL gResult = 0;
    while (true)
    {
        if (const auto eCode = Window::ManageProcesses())
        {
            return eCode.value();
        }
        DoFrame();
    }
    if (gResult < 0)
    {
        throw CHWND_LAST_EXCEPT();
    }
    return msg.wParam;
}

void App::DoFrame()
{
    auto dt = m_timer.Mark() *speedFactor;
    m_window.GetGraphics().SetCamera(camera.GetMatrix());
    m_window.GetGraphics().BeginFrame(0.0f, 0.0f, 0.0f);

    if (const auto& e = m_window.m_kbd.ReadKey())
    {
        if (e->GetType() == Keyboard::Event::Type::Press && e->GetCode() == VK_ESCAPE)
        {
            if (m_window.CursorEnabled())
            {
                m_window.DisableCursor();
                m_window.m_mouse.EnableRaw();
            }
            else
            {
                m_window.EnableCursor();
                m_window.m_mouse.DisableRaw();
            }
        }
    }
    if (!m_window.CursorEnabled())
    {
        if (m_window.m_kbd.KeyIsPressed('W'))
        {
            camera.Translate(0.0f, 0.0f, 1.0f);
        }
        if (m_window.m_kbd.KeyIsPressed('A'))
        {
            camera.Translate(-1.0f, 0.0f, 0.0f);
        }
        if (m_window.m_kbd.KeyIsPressed('S'))
        {
            camera.Translate(0.0f, 0.0f, -1.0f);
        }
        if (m_window.m_kbd.KeyIsPressed('D'))
        {
            camera.Translate(1.0f, 0.0f, 0.0f);
        }
        if (m_window.m_kbd.KeyIsPressed('R'))
        {
            camera.Translate(0.0f, 1.0f, 0.0f);
        }
        if (m_window.m_kbd.KeyIsPressed('F'))
        {
            camera.Translate(0.0f, -1.0f, 0.0f);
        }
        while(const auto& dt = m_window.m_mouse.ReadRawDelta())
        {
            camera.Rotate(dt->x, dt->y);
        }
    }

    pointLight.Bind(m_window.GetGraphics(),camera.GetMatrix());
    nano.Draw(m_window.GetGraphics());
    
    pointLight.Draw(m_window.GetGraphics());
    if (ImGui::Begin("Simulation Speed"))
    {
        ImGui::SliderFloat("Speed Factor", &speedFactor, 0.0f, 4.0f);
        ImGui::Text("Appliction Average %.1f ms/frame (%.3f FPS)", 1000 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();
    nano.ShowWindow();
    pointLight.SpawnControlWindow();
    camera.SpawnControlWindow();
    m_window.GetGraphics().EndFrame();
}

