#pragma once
#include "Base.h"


BEGIN(Engine)

class ENGINE_DLL CImGuiMgr final : public CBase
{
private:
	CImGuiMgr(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CImGuiMgr() = default;



public:
	HRESULT Initialize(const ENGINE_DESC& EngineDesc);
	void Render();

private:
	LPDIRECT3DDEVICE9				m_pGraphic_Device = { nullptr };
	ImGuiContext*					m_ImGuiContext = { nullptr };


public:
	static CImGuiMgr* Create(LPDIRECT3DDEVICE9 pGraphic_Device , const ENGINE_DESC& EngineDesc);
	virtual void Free() override;
};

END
