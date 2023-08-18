#pragma once
#include "ChiliWin.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <dxgidebug.h>
class DXGIInfoManager
{
public:
    DXGIInfoManager();
    ~DXGIInfoManager() = default;
    DXGIInfoManager(const DXGIInfoManager& dXGIInfoQueue) = delete;
    DXGIInfoManager& operator=(const DXGIInfoManager& dXGIInfoQueue) = delete;
    std::vector<std::string> GetMessages();
    void Set();
private:
    unsigned long long next = 0;
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue = nullptr;
};

