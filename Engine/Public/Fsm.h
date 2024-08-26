#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CFsm :
    public CComponent
{

private:
	CFsm(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CFsm(const CFsm& Prototype);
	virtual ~CFsm() = default;

public:

	class CGameObject*	Get_Owner() { return m_pOwner; }

public:
	virtual HRESULT Initialize_Prototype(_uint iNumMaxState);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta);
	virtual void Release_States();

public:
	HRESULT				Add_State(class CState* pState);
	void				Change_State(_uint iStateIndex, _int iStateDir = -1, void* pArg = nullptr);
	void				Set_State(_uint iStateIndex, void* pArg = nullptr);

private:
	vector<class CState*> m_States;						// ��� ���¸� ���� �ִ� ���� - �ε��� ���ٿ�
	_uint				  m_iNumMaxState = {};			// �� Fsm�� �������ִ� �ִ� State���� 

	class CState*	   m_pNowState = { nullptr };			//���� ����
	class CState*	   m_pPreState = { nullptr };			//���� ����

	class CGameObject* m_pOwner = { nullptr };

public:
	static CFsm* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumMaxState);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
