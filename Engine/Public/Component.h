#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device*  pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;


public:
	COMPONENT_TYPE Get_ComponentType() { return m_eComponentType; }

	const _wstring& Get_PrototypeName() { return m_strPrototypeName; }
	void		    Set_PrototypeName(const _wstring& PrototypeName) { m_strPrototypeName = PrototypeName; }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);


protected:
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = nullptr;

	class CGameInstance* m_pGameInstance = { nullptr };

	_bool						m_isCloned = { false };

	COMPONENT_TYPE				m_eComponentType = { COMPONENT_END };


	_wstring					m_strPrototypeName = L"";



public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END
