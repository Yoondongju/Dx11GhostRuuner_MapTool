#pragma once

#include "Base.h"


// CGraphic_Device ��� Ŭ�󿡼� �� �ʿ䰡 ���� 
// Engine�� GameObject���� �׷��ȵ���̽��� ��� �Ұű� ������ 
// ���� DLL�� ������(export) �ʿ䵵 Ŭ�󿡼� ����(import) �ʿ䵵 ����
// ���� ENGINE_DLL�� ������.


BEGIN(Engine) 

class CGraphic_Device final : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	// ��ġ �ʱ�ȭ ����

	// 1. ��ġ�� ������ ��ü�� ���� ����
	// 2. ��ġ�� �����ϵ��� ����(���� ���� �ľ�)
	// 3. ��ġ�� ������ ��ü�� ����

	HRESULT		Init_Device(const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void		Render_Begin();
	void		Render_End(HWND hWnd = nullptr);

	void		RenderText();


private:
	ID3D11Device*			m_pDevice = nullptr;		// 3���� �ش��ϴ� ��ü
	ID3D11DeviceContext*	m_pContext = nullptr;

	IDXGISwapChain*			m_pSwapChain = nullptr;             // Swap chain
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;		// Render target view
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;		// Depth stencil view

	// DirectWrite�� �ؽ�Ʈ ���̾ƿ��� �۲� ó���� ����ϰ�, Direct2D�� �ؽ�Ʈ�� �������ϴµ� ���ȴ�.

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