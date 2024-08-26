#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponents abstract : public CBase
{
protected:
	CComponents(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponents(const CComponents& Prototype);
	virtual ~CComponents() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);


protected:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = { nullptr };

public:
	virtual CComponents* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END
