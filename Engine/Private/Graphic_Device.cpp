#include "..\Public\Graphic_Device.h"

#include <wrl/client.h>

using namespace Engine;

CGraphic_Device::CGraphic_Device()
{
}

HRESULT CGraphic_Device::Init_Device(const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
    HRESULT hr = CreateDeviceAndSwapChain(EngineDesc);
    if (FAILED(hr))
    {
        return hr;
    }

    SetViewport(EngineDesc);

    // Pass back device and context
    *ppDevice = m_pDevice;
    *ppContext = m_pContext;

    Safe_AddRef(*ppDevice);         // 확실치 않음.. 일단레퍼런스 카운트 증가
    Safe_AddRef(*ppContext);        // 확실치 않음.. 일단레퍼런스 카운트 증가


    //if (FAILED(Init_Font()))
    //    return E_FAIL;

    
    return S_OK;
}

HRESULT CGraphic_Device::CreateDeviceAndSwapChain(const ENGINE_DESC& EngineDesc)
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL featureLevel;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = EngineDesc.iWinSizeX;
    sd.BufferDesc.Height = EngineDesc.iWinSizeY;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 144;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = EngineDesc.hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevels, 1, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, &featureLevel, &m_pContext);
    if (FAILED(hr))
    {
        // Handle error
        return hr;
    }

    ID3D11Texture2D* pBackBuffer = nullptr;

    // Create render target view
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr))
    {
        // Handle error
        return hr;
    }

    hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    if (FAILED(hr))
    {
        // Handle error
        return hr;
    }
    Safe_Release(pBackBuffer);   // TEXTURE2D로 렌더타겟뷰 다 만들엇으면 지워라

    // Create depth stencil texture and view
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.Width = EngineDesc.iWinSizeX;
    depthStencilDesc.Height = EngineDesc.iWinSizeY;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    ID3D11Texture2D* pDepthStencilBuffer = nullptr;
    hr = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &pDepthStencilBuffer);
    if (FAILED(hr))
    {
        // Handle error
        return hr;
    }

    hr = m_pDevice->CreateDepthStencilView(pDepthStencilBuffer, nullptr, &m_pDepthStencilView);
    if (FAILED(hr))
    {
        // Handle error
        return hr;
    }
    Safe_Release(pDepthStencilBuffer);       // TEXTURE2D로 렌더타겟뷰 다 만들엇으면 지워라



    // Bind render target view and depth stencil view to the output-merger stage
    m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


    return S_OK;
}

void CGraphic_Device::SetViewport(const ENGINE_DESC& EngineDesc)
{
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(EngineDesc.iWinSizeX);
    vp.Height = static_cast<FLOAT>(EngineDesc.iWinSizeY);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pContext->RSSetViewports(1, &vp);
}

HRESULT CGraphic_Device::Init_Font()
{
    

    return S_OK;
}


void CGraphic_Device::Render_Begin()
{
    float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_pContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
    m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CGraphic_Device::Render_End(HWND hWnd)
{
    m_pSwapChain->Present(1, 0);
}

void CGraphic_Device::RenderText()
{
    
}

CGraphic_Device* CGraphic_Device::Create(const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
    CGraphic_Device* pInstance = new CGraphic_Device();
    if (FAILED(pInstance->Init_Device(EngineDesc, ppDevice, ppContext)))
    {
        delete pInstance;
        return nullptr;
    }
    return pInstance;
}

void CGraphic_Device::Free()
{
    //m_spriteFont
   //m_spriteBatch
    

    Safe_Release(m_pDepthStencilView);
    Safe_Release(m_pRenderTargetView);
    Safe_Release(m_pSwapChain);            
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
