#pragma once
#include "Mouse.h"
#include "ChiliWin.h"
#include "ChilliException.h"
#include "Keyboard.h"
#include "Graphics.h"
#include<string>
#include<memory>
class Window
{
public:
    Window(int width,int heigth, LPCWSTR name);
    ~Window() noexcept;
    void SetTitle(const std::string& string);
    static std::optional<int> ManageProcesses();
    void EnableCursor();
    void DisableCursor();
    bool CursorEnabled();
    Graphics& GetGraphics();

    class Exception : public ChilliException
    {
        using ChilliException::ChilliException;
    protected:
        static const std::wstring TranslateErrorCode(HRESULT hr) noexcept;
    };

    class HrException : public Exception
    {
    public:
        HrException(int line, std::string file, HRESULT hr) noexcept;
        const char* what() const noexcept override;
        const std::string VGetType() const noexcept override;
        HRESULT GetErrorCode() const noexcept;
        const std::string GetErrorString() const noexcept;
    private:
        HRESULT m_hr;
    };

    class NoGfxException : public Exception
    {
    public:
        using Exception::Exception;
        const std::string VGetType() const noexcept override;
    };
    
private:
    class WindowClass
    {
    public:
        static LPCWSTR GetWindowClassName() noexcept;
        static HINSTANCE GetInstance() noexcept;
    private:
        WindowClass() noexcept;
        ~WindowClass();
        WindowClass(WindowClass&) = delete;
        WindowClass& operator =(const WindowClass&) = delete;
        HINSTANCE m_hInstance;
        static constexpr LPCWSTR wndClassName = L"WindowClass";
        static WindowClass wndClass;
    };
    int m_width = 0;
    int m_height = 0;
    HWND m_hWnd = nullptr;
    bool cursorEnabled = true;
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    void ShowCursor();
    void HideCursor();
    void ConfineCursor();
    void FreeCursor();
    void DisableImGuiMouse();
    void EnableImGuiMouse();
public:
    Mouse m_mouse;
    Keyboard m_kbd;
    std::unique_ptr<Graphics> m_pGraphics;
    std::vector<char> rawBuffer;
};

#define CHWND_EXCEPT(hr) Window::HrException(__LINE__,__FILE__,hr)
#define CHWND_LAST_EXCEPT() Window::HrException(__LINE__,__FILE__,GetLastError())