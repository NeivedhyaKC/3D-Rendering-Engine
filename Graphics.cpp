#include "Graphics.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <DirectXMath.h>
#include <sstream>
#include <d3dcompiler.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

Graphics::Graphics(HWND hWnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;
    swapChainDesc.BufferCount = 1;

    UINT swapCreateFlags = 0;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG

    HRESULT hr;
    GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &pSwapChain,
        &pDevice,
        nullptr,
        &pDeviceContext
    ));

    ImGui_ImplDX11_Init(pDevice.Get(), pDeviceContext.Get());
    Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer = nullptr;
    GFX_THROW_INFO(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer));
    GFX_THROW_INFO(pDevice->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        &pRenderTargetView
    ));

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
    D3D11_DEPTH_STENCIL_DESC dssDesc = { 0 };
    dssDesc.DepthEnable = TRUE;
    dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
    GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dssDesc, &pDepthStencilState));

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil = nullptr;
    D3D11_TEXTURE2D_DESC dsDesc = { 0 };
    dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsDesc.ArraySize = 1u;
    dsDesc.MipLevels = 1u;
    dsDesc.Usage = D3D11_USAGE_DEFAULT;
    dsDesc.SampleDesc.Count = 1u;
    dsDesc.SampleDesc.Quality = 0u;
    dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    dsDesc.Width = width;
    dsDesc.Height = height;
    dsDesc.MiscFlags = 0u;
    GFX_THROW_INFO(pDevice->CreateTexture2D(&dsDesc, nullptr, &pDepthStencil));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0u;
    GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvDesc, &pDepthStencilView));

    D3D11_VIEWPORT vp = { 0 };
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    pDeviceContext->RSSetViewports(1u, &vp);

    GFX_THROW_INFO_ONLY(pDeviceContext->OMSetRenderTargets(1u, pRenderTargetView.GetAddressOf(),pDepthStencilView.Get()));
}

Graphics::~Graphics()
{
    ImGui_ImplDX11_Shutdown();
}

const DirectX::XMMATRIX& Graphics::GetCamera()
{
    return camera;
}

void Graphics::SetCamera(DirectX::XMMATRIX transform)
{
    camera = transform;
}

const DirectX::XMMATRIX& Graphics::GetProjection()
{
    return projection;
}

void Graphics::SetProjection(DirectX::XMMATRIX transform)
{
    projection = transform;
}

void Graphics::DrawIndexed(int count) noexcept(!IS_DEBUG)
{
    GFX_THROW_INFO_ONLY(pDeviceContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::EndFrame()
{
    HRESULT hr;
#ifndef NDEBUG
    infoManager.Set();
#endif 
    if (imGuiEnabled)
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    if (FAILED(hr = pSwapChain->Present(1u, 0u)))
    {
        hr = DXGI_ERROR_DEVICE_REMOVED;
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw GFX_DEVICE_REMOVE_EXCEPT(pDevice->GetDeviceRemovedReason());
        }
        else
        {
            GFX_THROW_INFO(hr);
        }
    }
}

void Graphics::DrawTestTriangle(float angle, float x, float z)
{
    
}

void Graphics::BeginFrame(float red, float blue, float green)
{
    if (imGuiEnabled)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }
    float color[] = { red, blue, green, 1.0f };
    pDeviceContext->ClearRenderTargetView(pRenderTargetView.Get(), color);
    pDeviceContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::SetImGuiEnabled(bool imGuiEnable)
{
    imGuiEnabled = imGuiEnable;
}

bool Graphics::IsImGuiEnabled()
{
    return imGuiEnabled;
}

Graphics::HrException::HrException(int line, std::string file, HRESULT hr, std::vector<std::string> messages)
    :Exception(line,file),
     hr(hr)
{
    for (const auto& m : messages)
    {
        info += m;
        info.push_back('\n');
    }
    if (!info.empty())
    {
        info.pop_back();
    }
}

const char* Graphics::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type]" << VGetType() << std::endl
        << "[ErrorCode]" << std::hex << GetErrorCode() << "{" << std::dec << GetErrorCode() << "}" << std::endl
        << "[ErrorString] " << GetErrorString() << std::endl
        << "[ErrorDescription]" << GetErrorDescription() << std::endl
        << GetOriginString() << std::endl;
    if (!info.empty())
    {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    };
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

const std::string Graphics::HrException::VGetType() const noexcept
{
    return "Chilli Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
    return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
    std::wstring wstr(DXGetErrorString(hr));
    std::string str(wstr.begin(), wstr.end());
    return str;
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
    wchar_t wchar[512] = {};
    DXGetErrorDescriptionW(hr, wchar, sizeof(wchar));
    std::wstring wstr(wchar);
    return std::string(wstr.begin(),wstr.end());
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
    return info;
}

const std::string Graphics::DeviceRemovedException::VGetType() const noexcept
{
    return "Chilli Graphics Exception [Device Removed] DXGI_ERROR_DEVICE_REMOVED";
}

Graphics::InfoException::InfoException(int line, std::string file, std::vector<std::string> messages):
    Exception(line,file)
{
    for (std::string& message : messages)
    {
        info += message;
        info.push_back('\n');
    }
    if (!info.empty())
    {
        info.pop_back();
    }
}

const std::string Graphics::InfoException::VGetType() const noexcept
{
    return "Info Exception";
}

const char* Graphics::InfoException::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << VGetType() << std::endl
        << "[ErrorInfo] " << GetErrorInfo() << std::endl
        << GetOriginString() << std::endl;
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
    return info;
}
