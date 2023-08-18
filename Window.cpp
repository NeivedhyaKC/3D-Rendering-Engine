#include "Window.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include <sstream>
#include "resource.h"

Window::Window(int width, int height, LPCWSTR name):
    m_width(width),
    m_height(height)
{
    RECT windowRect;
    windowRect.left = 100;
    windowRect.top = 100;
    windowRect.right = width + windowRect.left;
    windowRect.bottom = windowRect.top + height;
    if (AdjustWindowRect(&windowRect, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE) == 0)
    {
        throw CHWND_LAST_EXCEPT();
    }
    m_hWnd = CreateWindowExW(0, Window::WindowClass::GetWindowClassName(), name,
        WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        200, 200, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        nullptr, nullptr, WindowClass::GetInstance(), this);
    if (m_hWnd == nullptr)
    {
        throw CHWND_LAST_EXCEPT();
    }
    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    
    ImGui_ImplWin32_Init(m_hWnd);
    m_pGraphics = std::make_unique<Graphics>(m_hWnd,width,height);

    RAWINPUTDEVICE rid;
    rid.dwFlags = 0;
    rid.hwndTarget = nullptr;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    if (RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)) == FALSE)
    {
        throw CHWND_LAST_EXCEPT();
    }
}

Window::~Window() noexcept
{
    DestroyWindow(m_hWnd);
    ImGui_ImplWin32_Shutdown();
}

void Window::SetTitle(const std::string& string)
{
    std::wstring wstr(string.begin(),string.end());
    if (SetWindowText(m_hWnd, wstr.c_str())==0)
    {
        throw CHWND_LAST_EXCEPT();
    }
}

std::optional<int> Window::ManageProcesses()
{
    MSG msg = { 0 };
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if(msg.message == WM_QUIT)
        {
            return msg.wParam;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return {};
}

void Window::EnableCursor()
{
    cursorEnabled = true;
    ShowCursor();
    EnableImGuiMouse();
    FreeCursor();
}

void Window::DisableCursor()
{
    cursorEnabled = false;
    HideCursor();
    DisableImGuiMouse();
    ConfineCursor();
}

bool Window::CursorEnabled()
{
    return cursorEnabled;
}

Graphics& Window::GetGraphics()
{
    if (m_pGraphics == nullptr)
    {
        throw Window::NoGfxException(__LINE__,__FILE__);
    }
    return *m_pGraphics;
}

Window::WindowClass Window::WindowClass::wndClass;

Window::HrException::HrException(int line, std::string file, HRESULT hr) noexcept:
    Exception(line, file),
    m_hr(hr)
{

}

const char* Window::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << "[ErrorCode] " << GetErrorCode() << std::endl
        << "[ErrorDescription] " << GetErrorString() << std::endl
        << GetOriginString() << std::endl;
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

const std::string Window::HrException::VGetType() const noexcept
{
    return "Window::Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
    return m_hr;
}

const std::string Window::HrException::GetErrorString() const noexcept
{
    std::wstring wstring(TranslateErrorCode(m_hr));
    return std::string(wstring.begin(), wstring.end());
}

const std::wstring Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
    wchar_t* pMsgBuf = nullptr;
    const DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<wchar_t*>( &pMsgBuf), 0, nullptr
    );
    if (nMsgLen == 0)
    {
        return L"Unidentified error code";
    }
    std::wstring errorString = pMsgBuf;
    LocalFree(pMsgBuf);
    return errorString;
}

LPCWSTR Window::WindowClass::GetWindowClassName() noexcept
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.m_hInstance;
}

Window::WindowClass::WindowClass() noexcept :
    m_hInstance(GetModuleHandle(nullptr))
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = HandleMsgSetup;
    wc.lpszClassName = (LPCWSTR) GetWindowClassName();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetInstance();
    wc.hCursor = nullptr;
    wc.hIcon = reinterpret_cast<HICON>(LoadImageW(GetInstance(), MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 32, 32, 0));
    wc.lpszMenuName = nullptr;
    wc.hIconSm = reinterpret_cast<HICON>(LoadImageW(GetInstance(), MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16 ,16, 0));
    wc.hbrBackground = nullptr;
    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(GetWindowClassName(), GetInstance());
}

LRESULT Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        Window* thisWindow = reinterpret_cast<Window*>(pCreateStruct->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisWindow));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

        return thisWindow->HandleMsg(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return window->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }
    const auto& imio = ImGui::GetIO();
    switch (msg)
    {
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
        break;
    }
    case WM_KILLFOCUS:
    {
        m_kbd.ClearState();
        break;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        if (imio.WantCaptureKeyboard)
        {
            break;
        }
        if (!(lParam & 0x40000000) || m_kbd.IsAutoRepeatEnabled())
        {
            m_kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
        }
        break;
    }
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        if (imio.WantCaptureKeyboard)
        {
            break;
        }
        m_kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
        break;
    }
    case WM_CHAR:
    {
        if (imio.WantCaptureKeyboard)
        {
            break;
        }
        m_kbd.OnChar(static_cast<unsigned char>(wParam));
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (imio.WantCaptureMouse)
        {
            break;
        }
        const POINTS pts = MAKEPOINTS(lParam);
        if ((pts.x > m_width || pts.x <0 || pts.y>m_height || pts.y < 0) && (!m_mouse.IsLeftButtonPressed() && !m_mouse.IsRightButtonPressed()))
        {
            ReleaseCapture();
            m_mouse.OnMouseLeave(pts.x, pts.y);
        }
        else if (pts.x < m_width && pts.x >0 && pts.y < m_height && pts.y >0 && !m_mouse.IsInWindow())
        {
            SetCapture(m_hWnd);
            m_mouse.OnMouseEnter(pts.x, pts.y);
        }
        else
        {
            m_mouse.OnMouseMove(pts.x, pts.y);
        }
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (!cursorEnabled)
        {
            ConfineCursor();
            HideCursor();
        }
        if (imio.WantCaptureMouse)
        {
            break;
        }
        const POINTS pts = MAKEPOINTS(lParam);
        m_mouse.OnLeftPressed(pts.x, pts.y);
        break;
    }
    case WM_LBUTTONUP:
    {
        if (imio.WantCaptureMouse)
        {
            break;
        }
        const POINTS pts = MAKEPOINTS(lParam);
        m_mouse.OnLeftReleased(pts.x, pts.y);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        if (imio.WantCaptureMouse)
        {
            break;
        }
        const POINTS pts = MAKEPOINTS(lParam);
        m_mouse.OnRightPressed(pts.x, pts.y);
        break;
    }
    case WM_RBUTTONUP:
    {
        if (imio.WantCaptureMouse)
        {
            break;
        }
        const POINTS pts = MAKEPOINTS(lParam);
        m_mouse.OnRightReleased(pts.x, pts.y);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        if (imio.WantCaptureMouse)
        {
            break;
        }
        const POINTS pts = MAKEPOINTS(lParam);
        int wheelDelta = (GET_WHEEL_DELTA_WPARAM(wParam));
        m_mouse.OnWheelMove(pts.x, pts.y, wheelDelta);
    }
    case WM_ACTIVATE:
    {
        if (!cursorEnabled)
        {
            if (wParam & WA_ACTIVE)
            {
                HideCursor();
                ConfineCursor();
            }
            else
            {
                ShowCursor();
                FreeCursor();
            }
        }
    }
    case WM_INPUT:
    {
        if (!m_mouse.RawCursorEnabled())
        {
            break;
        }
        unsigned int size;
        if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1)
        {
            break;
        }
        rawBuffer.resize(size);
        if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
        {
            break;
        }
        auto& ri = *reinterpret_cast<RAWINPUT*>(rawBuffer.data());
        if (ri.header.dwType == RIM_TYPEMOUSE && (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
        {
            m_mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
        }
        break;
    }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::ShowCursor()
{
    while(::ShowCursor(TRUE) < 0);
}

void Window::HideCursor()
{
    while (::ShowCursor(FALSE) >= 0);
}

void Window::ConfineCursor()
{
    RECT rect;
    GetClientRect(m_hWnd, &rect);
    MapWindowPoints(m_hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
    ClipCursor(&rect);
}

void Window::FreeCursor()
{
    ClipCursor(nullptr);
}

void Window::DisableImGuiMouse()
{
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void Window::EnableImGuiMouse()
{
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

const std::string Window::NoGfxException::VGetType() const noexcept
{
    return "No Grpahics instance";
}
