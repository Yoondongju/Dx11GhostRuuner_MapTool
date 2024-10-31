#pragma once

#include "Base.h"
#include "Transform.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */
BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_uint iModelListType = 0;
		_uint iModelNum = 0;

		_uint iObjectType = 0;

	} GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device*  pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);

	virtual ~CGameObject() = default;


public:
	_bool	Get_Dead() { return m_isDead; }
	void	Set_Dead(_bool isDead) { m_isDead = isDead; }


	void		Set_PrototypeName(const _wstring& strPrototypeName) { m_strPrototypeName = strPrototypeName; }
	_wstring	Get_PrototypeName() { return m_strPrototypeName; }


	void		Set_LayerName(const _wstring& strLayerName) { m_strLayerName = strLayerName; }
	_wstring    Get_LayerName() { return m_strLayerName; }


public:
	virtual class CModel*		Get_Model() { return nullptr; }
	virtual class CFsm*			Get_Fsm() { return nullptr; }
	virtual class CNavigation*  Get_Navigation() { return nullptr; }


	_uint		Get_ModelListType() { return m_iModelListType; }
	_uint		Get_ModelNum() { return m_iModelNum; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Height() { return S_OK; }

public:
	class CComponent* Find_Component(const _wstring & strComponentTag);
	class CComponent* Find_Component(COMPONENT_TYPE eComponentTag);

	class CTransform* Get_Transform() { return m_pTransformCom; }


protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = nullptr;
	

	class CGameInstance*	m_pGameInstance = { nullptr };

	_uint					m_iModelListType = { 0 };		// 내가 픽한 모델리스트의 타입
	_uint					m_iModelNum = 0;

	_wstring				m_strPrototypeName = L"";		// 맵툴에서 저장불러오기할때 도저히 판단기준을 잡을수없어서 만듬
	_wstring				m_strLayerName = L"";


	_bool					m_isDead = { false };


	_uint					m_iObjectType = {};		// 데코,스태틱,다이나믹



protected:
	map<const _wstring, class CComponent*>	m_Components;
	class CTransform* m_pTransformCom = nullptr;

	HRESULT	Add_Component(_uint iLevelIndex, const _wstring strPrototypeTag, const _wstring & strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

