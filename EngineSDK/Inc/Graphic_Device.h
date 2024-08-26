#pragma once

#include "Base.h"


// CGraphic_Device 얘는 클라에서 알 필요가 없다 
// Engine의 GameObject에서 그래픽디바이스를 사용 할거기 때문에 
// 따라서 DLL을 내보낼(export) 필요도 클라에서 적용(import) 필요도 없다
// 따라서 ENGINE_DLL을 뗄것임.


BEGIN(Engine) 

class CGraphic_Device final : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	// 장치 초기화 과정

	// 1. 장치를 조사할 객체를 먼저 생성
	// 2. 장치를 조사하도록 지시(지원 수준 파악)
	// 3. 장치를 제어할 객체를 생성

	HRESULT		Init_Device(const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void		Render_Begin();
	void		Render_End(HWND hWnd = nullptr);

	void		RenderText();


private:
	ID3D11Device*			m_pDevice = nullptr;		// 3번에 해당하는 객체
	ID3D11DeviceContext*	m_pContext = nullptr;

	IDXGISwapChain*			m_pSwapChain = nullptr;             // Swap chain
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;		// Render target view
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;		// Depth stencil view

	// DirectWrite는 텍스트 레이아웃과 글꼴 처리를 담당하고, Direct2D는 텍스트를 렌더링하는데 사용된다.

	std::unique_ptr<SpriteBatch> m_spriteBatch;
	std::unique_ptr<SpriteFont> m_spriteFont;

private:
	HRESULT CreateDeviceAndSwapChain(const ENGINE_DESC& EngineDesc);
	void SetViewport(const ENGINE_DESC& EngineDesc);

	HRESULT	Init_Font();
	//HRESULT Create_Text_LayoutAndFormat();

public:
	static CGraphic_Device* Create(const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	virtual void Free() override;
};

END