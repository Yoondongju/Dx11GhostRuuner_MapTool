#include "../Public/Layer.h"

#include "../Public/GameObject.h"
#include "../Public/Transform.h"

CLayer::CLayer()
{
}

_uint CLayer::Get_GamObjectSize()
{
    return m_GameObjects.size();
}


CGameObject* CLayer::IsObjectAtPosition(_float3 vPosition)
{
    for (auto& pGameObj : m_GameObjects)
    {
        CTransform* pTransform = dynamic_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));
        _vector    vMyPosition = pTransform->Get_State(CTransform::STATE_POSITION); 
         
        // vMyPosition은 지금 지형을 탄위치야 , vPosition은 걍 순수좌표고  
        // 그냥 y를 비교하지마?
        // vPosition은 월드상의 위치를 의미해
        // 근데 오브젝트가 지형을 타는과정에서 연산으로인해 부동소수점의 오차가 발생했어
   
        
        

        float fx = fabs(XMVectorGetX(vMyPosition) - vPosition.x);
        float fz = fabs(XMVectorGetZ(vMyPosition) - vPosition.z);

        if (fabs(XMVectorGetX(vMyPosition) - vPosition.x) < EPSILON &&
            fabs(XMVectorGetZ(vMyPosition) - vPosition.z) < EPSILON )
            return pGameObj;
    }
    return nullptr;
}

CGameObject* CLayer::IsObjectAtPosition_GoodVerdict(_float3 vPosition)
{
    for (auto& pGameObj : m_GameObjects)
    {
        CTransform* pTransform = dynamic_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));
        _vector    vMyPosition = pTransform->Get_State(CTransform::STATE_POSITION);

        // vMyPosition은 지금 지형을 탄위치야 , vPosition은 걍 순수좌표고  
        // 그냥 y를 비교하지마?
        // vPosition은 월드상의 위치를 의미해
        // 근데 오브젝트가 지형을 타는과정에서 연산으로인해 부동소수점의 오차가 발생했어

        float fx = fabs(XMVectorGetX(vMyPosition) - vPosition.x);
        float fz = fabs(XMVectorGetZ(vMyPosition) - vPosition.z);

        if (fabs(XMVectorGetX(vMyPosition) - vPosition.x) < 0.4f &&
            fabs(XMVectorGetZ(vMyPosition) - vPosition.z) < 0.4f)
            return pGameObj;
    }
    return nullptr;
}

CGameObject* CLayer::ObjectAtPosition(_float3 vPosition)
{
    for (auto& pGameObj : m_GameObjects)
    {
        CTransform* pTransform = dynamic_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));
        _vector    vMyPosition = pTransform->Get_State(CTransform::STATE_POSITION);

        // vMyPosition은 지금 지형을 탄위치야 , vPosition은 걍 순수좌표고  
        // 그냥 y를 비교하지마?
        // vPosition은 월드상의 위치를 의미해
        // 근데 오브젝트가 지형을 타는과정에서 연산으로인해 부동소수점의 오차가 발생했어

        float fx = fabs(XMVectorGetX(vMyPosition) - vPosition.x);
        float fz = fabs(XMVectorGetZ(vMyPosition) - vPosition.z);

        if (fabs(XMVectorGetX(vMyPosition) - vPosition.x) < EPSILON &&
            fabs(XMVectorGetZ(vMyPosition) - vPosition.z) < EPSILON)
            return pGameObj;
    }

    return nullptr;
}

HRESULT CLayer::Add_GameObj(CGameObject* pGameObject)
{
    if (nullptr == pGameObject)
        return E_FAIL;

    m_GameObjects.emplace_back(pGameObject);

    return S_OK;
}

void CLayer::Clear_Layer()
{
    for (auto pGameObj : m_GameObjects)
    {
        Safe_Release(pGameObj);
    }
    m_GameObjects.clear();
}

HRESULT CLayer::Priority_Update(_float fTimeDelta)
{
    for (auto& pGameObj : m_GameObjects)
        pGameObj->Priority_Update(fTimeDelta);

    return S_OK;
}

HRESULT CLayer::Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (nullptr != pGameObject)
        {
            pGameObject->Update(fTimeDelta);
        }
    }

    return S_OK;
}

HRESULT CLayer::Late_Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (nullptr != pGameObject)
        {
            pGameObject->Late_Update(fTimeDelta);
        }
    }

    return S_OK;
}

CComponent* CLayer::Find_Component(const _wstring& strComponentTag, _uint iIndex)
{
    if (iIndex >= m_GameObjects.size())
        return nullptr;

    auto	iter = m_GameObjects.begin();

    for (size_t i = 0; i < iIndex; i++)
        ++iter;

    return (*iter)->Find_Component(strComponentTag);
}

CGameObject* CLayer::Find_Object(_uint iIndex)
{
    if (iIndex >= m_GameObjects.size())
        return nullptr;

    auto	iter = m_GameObjects.begin();

    for (size_t i = 0; i < iIndex; i++)
        ++iter;

    return (*iter);
}


CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    __super::Free();

    for (auto& pGameObject : m_GameObjects)
        Safe_Release(pGameObject);

    m_GameObjects.clear();
}
