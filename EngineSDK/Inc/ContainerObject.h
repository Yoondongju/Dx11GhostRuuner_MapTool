#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
protected:
	CContainerObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual ~CContainerObject() = default;

public:
	class CPartObject*		Get_Part(_uint iPartIndex) { return m_Parts[iPartIndex]; }
	_uint					Get_PartSize() { return m_Parts.size(); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	vector<class CPartObject*>			m_Parts;	// Create�����̳� ��������� ä���ִ°��̾ƴ϶� Add_PartObject������ ä���ִ� ����������ص� ����������

protected:
	HRESULT Add_PartObject(_uint iPartID, const _wstring & strPrototypeTag , void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END