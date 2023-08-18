#include "DXGIInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <memory>

#pragma comment(lib,"dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr=(hrcall))) throw Graphics::HrException(__LINE__,__FILE__,hr)

DXGIInfoManager::DXGIInfoManager()
{
    typedef HRESULT(WINAPI* DxgiDebugInterface)(REFIID, void**);

    const auto hModDxgiDebug = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hModDxgiDebug == nullptr)
    {
        throw CHWND_LAST_EXCEPT();
    }

    DxgiDebugInterface dxgiDebugInterface = reinterpret_cast<DxgiDebugInterface>(reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug,
        "DXGIGetDebugInterface")));
    if (dxgiDebugInterface == nullptr)
    {
        throw CHWND_LAST_EXCEPT();
    }
    HRESULT hr;
    GFX_THROW_NOINFO(dxgiDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue));
}

std::vector<std::string> DXGIInfoManager::GetMessages()
{
    std::vector<std::string> messages;
    const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
    for (auto i = next; i < end; i++)
    {
        HRESULT hr;
        SIZE_T messageLength = 0;
        GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageW(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
        auto bytes = std::make_unique<byte[]>(messageLength);
        auto message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
        GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageW(DXGI_DEBUG_ALL, i, message, &messageLength));
        messages.emplace_back(message->pDescription);
    }
    return messages;
}

void DXGIInfoManager::Set()
{
    next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}


