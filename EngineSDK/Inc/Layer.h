#pragma once
#include "Base.h"

BEGIN(Engine)
class CLayer final : public CBase
{
private:
    CLayer();
    virtual ~CLayer() = default;


public:
    _uint Get_GamObjectSize();
    list<class CGameObject*>& Get_GamObjects() { return m_GameObjects; }

public:

    class CGameObject* ObjectAtPosition(_float3 vPosition);
    class CGameObject* IsObjectAtPosition(_float3 vPosition);

    class CGameObject* IsObjectAtPosition_GoodVerdict(_float3 vPosition);

public:
    HRESULT   Add_GameObj(class CGameObject* pGameObject);
    void Clear_Layer();
  

    HRESULT Priority_Update(_float fTimeDelta);
    HRESULT Update(_float fTimeDelta);
    HRESULT Late_Update(_float fTimeDelta);



public:
    class CComponent*  Find_Component(const _wstring& strComponentTag, _uint iIndex);    // 특정 인덱스에 존재하는 한놈만 찾는거
    class CGameObject* Find_Object(_uint iIndex);

private:
    list<class CGameObject*>      m_GameObjects;

public:
    static CLayer* Create();
    virtual void Free() override;
};
END

