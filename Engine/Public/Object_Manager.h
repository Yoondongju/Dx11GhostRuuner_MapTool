#pragma once
#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	// strLayerTag : 아이템
	// strPrototypeTag: 무기아이템, 방어구아이템 이런식
	HRESULT	Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	
	
public:
	class CGameObject* Clone_GameObject(const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);
	class CLayer*     Find_Layer(_uint iLevelIndx, const _wstring& strLayerTag);
	list<class CGameObject*>& Get_GameObjects(_uint iLevelIndx, const _wstring& strLayerTag);



	class CGameObject* ObjectAtPosition(_uint iLevelIndex, const _wstring& strLayerTag , _float3 vPosition);

	class CGameObject* IsObjectAtPosition(_uint iLevelIndex, const _wstring& strLayerTag, _float3 vPosition);
	class CGameObject* IsObjectAtPosition_GoodVerdict(_uint iLevelIndex, const _wstring& strLayerTag, _float3 vPosition);


public:
	class CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);
	class CGameObject* Find_Player();
	class CGameObject* Find_Camera();




private:
	// 원형 객체 보관
	map<const _wstring, class CGameObject*>		m_Prototypes;	

	// 레벨별로 사본 객체를 보관할 용도
	_uint										m_iNumLevels = {};
	map<const _wstring, class CLayer*>*			m_pLayers = { nullptr };
	typedef map<const _wstring, class CLayer*>  LAYERS;			


private:
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);
	


public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END

