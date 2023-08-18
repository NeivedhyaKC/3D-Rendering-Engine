#include"App.h"

std::wstring ConvertStringToWString(const char* string)
{
    std::string str = string;
    std::wstring wstr(str.begin(), str.end());
    return wstr;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
    try
    {
        App app;
        return app.Go();
    }
    catch (const ChilliException& e)
    {
        std::wstring eWhat = ConvertStringToWString(e.what());
        std::wstring eType = ConvertStringToWString(e.VGetType().c_str()).c_str();
        MessageBox(nullptr, eWhat.c_str(), eType.c_str(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e)
    {
        MessageBox(nullptr, ConvertStringToWString(e.what()).c_str(), L"std::exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBox(nullptr, L"No detail available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
}
