#pragma once
#include"ChiliWin.h"
#include "Camera.h"
#include<d3d11.h>
#include "dxerr.h"
#include<string>
#include<vector>
#include "ChilliException.h"
#include "DXGIInfoManager.h"
#include <wrl.h>
#include<DirectXMath.h>

class Graphics
{
public:
    friend class Bindable;
    class Exception : public ChilliException
    {
        using ChilliException::ChilliException;
    };
    
    class HrException : public Exception
    {
    public:
        HrException(int line, std::string file, HRESULT hr, std::vector<std::string> messages = {});
        const char* what() const noexcept override;
        const std::string VGetType() const noexcept override;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;
        std::string GetErrorDescription() const noexcept;
        std::string GetErrorInfo() const noexcept;

    private:
        HRESULT hr;
        std::string info;
    };

    class DeviceRemovedException : public HrException
    {
        using HrException::HrException;
    public:
        const std::string VGetType() const noexcept override;
        std::string reason;
    };

    class InfoException : public Exception
    {
    public:
        InfoException(int line, std::string file, std::vector<std::string> messages);
        const std::string VGetType() const noexcept override;
        const char* what() const noexcept override;
        std::string GetErrorInfo() const noexcept;
    private:
        std::string info;
    };

    Graphics(HWND hWnd,int width, int height);
    ~Graphics();
    Graphics(const Graphics& graphics) = delete;
    Graphics& operator=(const Graphics& graphics) = delete;
    const DirectX::XMMATRIX& GetCamera();
    void SetCamera(DirectX::XMMATRIX transform);
    const DirectX::XMMATRIX& GetProjection();
    void SetProjection( DirectX::XMMATRIX transform);
    void DrawIndexed(int count) noexcept(!IS_DEBUG);
    void EndFrame();
    void DrawTestTriangle(float angle, float x, float y);
    void BeginFrame(float red, float blue, float green);
    void SetImGuiEnabled(bool imGuiEnabled);
    bool IsImGuiEnabled();
private:
    bool imGuiEnabled = true;
#ifndef  NDEBUG
    DXGIInfoManager infoManager;
#endif // ! NDEBUG
    DirectX::XMMATRIX camera;
    DirectX::XMMATRIX projection;

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;
    Microsoft::WRL::ComPtr <IDXGISwapChain> pSwapChain = nullptr;
    Microsoft::WRL::ComPtr <ID3D11Device> pDevice = nullptr;
    Microsoft::WRL::ComPtr <ID3D11RenderTargetView> pRenderTargetView = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;
};

#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr=(hrcall))) throw Graphics::HrException(__LINE__,__FILE__,hr)

#ifndef NDEBUG
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException(__LINE__,__FILE__,v);}}
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hr=(hrcall)))  throw Graphics::HrException(__LINE__,__FILE__,hr,infoManager.GetMessages())
#define GFX_DEVICE_REMOVE_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__,__FILE__,hr,infoManager.GetMessages())
#else
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVE_EXCEPT(hr) throw Graphics::DeviceRemovedException(__LINE__,__FILE__,hr)
#endif // !NDEBUG
#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else
#define INFOMAN(gfx) HRESULT hr; DXGIInfoManager& infoManager = GetInfoManager((gfx))
#endif
