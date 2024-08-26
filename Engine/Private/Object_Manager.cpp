#include "../Public/Object_Manager.h"

#include "../Public/GameObject.h"
#include "../Public/Layer.h"


CObject_Manager::CObject_Manager()
{
}


HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
    if (nullptr != m_pLayers)
        return E_FAIL;

    m_pLayers = new LAYERS[iNumLevels];
    m_iNumLevels = iNumLevels;

    return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
    CGameObject* pGameObj = Find_Prototype(strPrototypeTag);

    if (nullptr != pGameObj)      // 이미 해당 PrototypeTag명을 가진 객체가 존재함// Prototype에선 오직 하나의 PrototypeTag명을 가진애만 추가하는거기에 실패에욧
    {
        Safe_Release(pGameObj);
        m_Prototypes.erase(strPrototypeTag);
    }

    m_Prototypes.emplace(strPrototypeTag, pPrototype);
    return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
    if (iLevelIndex >= m_iNumLevels)
        return E_FAIL;


    CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
    if (nullptr == pPrototype)    // 내가 추가할객체의 프로토타입이 없을경우
        return E_FAIL;           // 이것도 그냥 리턴시켜야할지 assert를 걸어야할지 추가를 해줘야할지 모르겠다


    CGameObject* pGameObject = pPrototype->Clone(pArg);
    if (nullptr == pGameObject)     // 클론이 nullptr 떨어지는경우가 있나?   
        return E_FAIL;



    CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

    if (nullptr == pLayer)         // 레이어가 잇다는소리
    {
        pLayer = CLayer::Create(); // 레이어를 생성할때 strLayerTag를 받는게 더 좋지 않았을까? 
        pLayer->Add_GameObj(pGameObject);
        m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
    }
    else
    {
        //if (L"Layer_BackGround_Terrain" == strLayerTag)   // 지형은 사본도 하나여야해
        //{
        //    pLayer->Clear_Layer();
        //    pLayer->Add_GameObj(pGameObject);
        //    m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
        //}
        //else
        {
            pLayer->Add_GameObj(pGameObject);
            m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
        }
    }

    pGameObject->Set_PrototypeName(strPrototypeTag);
    pGameObject->Set_LayerName(strLayerTag);

    return S_OK;
}



HRESULT CObject_Manager::Priority_Update(_float fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pLayers[i])
        {
            Pair.second->Priority_Update(fTimeDelta);
        }
    }
    return S_OK;
}

HRESULT CObject_Manager::Update(_float fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        // 자기가 들고있는 레벨만 돌리는게 아님? 이라고 생각했는데
        // 어짜피 자기가 들고있는 레벨에 객체가 없으면 두번째 for문이 안돌거임
        // 따라서 레벨의 갯수만큼만 루프를 돈다. 
        for (auto& Pair : m_pLayers[i])
        {
            // ex)  무기레이어 Object에 검종류무기, 활종류무기 이런게 있으니까 
            Pair.second->Update(fTimeDelta);
        }
    }
    return S_OK;
}

HRESULT CObject_Manager::Late_Update(_float fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; ++i)   // 레벨 별로 루프돌고
    {
        for (auto& Pair : m_pLayers[i])
        {
            Pair.second->Late_Update(fTimeDelta);
        }
    }
    return S_OK;
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
    for (auto& Pair : m_pLayers[iLevelIndex])
        Safe_Release(Pair.second);

    m_pLayers[iLevelIndex].clear();
}

CGameObject* CObject_Manager::Clone_GameObject(const _wstring& strPrototypeTag, void* pArg)
{
    CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
    if (nullptr == pPrototype)
        return nullptr;

    CGameObject* pGameObject = pPrototype->Clone(pArg);
    if (nullptr == pGameObject)
        return nullptr;

    return pGameObject;
}

CComponent* CObject_Manager::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
    CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->Find_Component(strComponentTag, iIndex);
}

CGameObject* CObject_Manager::Find_Prototype(const _wstring& strPrototypeTag)
{
    auto iter = m_Prototypes.find(strPrototypeTag);

    if (iter == m_Prototypes.end())
        return nullptr;

    return iter->second;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndx, const _wstring& strLayerTag)
{
    auto iter = m_pLayers[iLevelIndx].find(strLayerTag);

    if (iter == m_pLayers[iLevelIndx].end())
        return nullptr;

    return iter->second;
}

list<class CGameObject*>& CObject_Manager::Get_GameObjects(_uint iLevelIndx, const _wstring& strLayerTag)
{
    static list<class CGameObject*> emptyList;

    CLayer* pLayer = Find_Layer(iLevelIndx, strLayerTag);
    if (nullptr == pLayer)
        return emptyList;


    return pLayer->Get_GamObjects();
}

CGameObject* CObject_Manager::IsObjectAtPosition(_uint iLevelIndex, const _wstring& strLayerTag , _float3 vPosition)
{
    auto iter = m_pLayers[iLevelIndex].find(strLayerTag);
   
    if (iter == m_pLayers[iLevelIndex].end())
        return false;

    return (iter->second)->IsObjectAtPosition(vPosition);
}

CGameObject* CObject_Manager::IsObjectAtPosition_GoodVerdict(_uint iLevelIndex, const _wstring& strLayerTag, _float3 vPosition)
{
    auto iter = m_pLayers[iLevelIndex].find(strLayerTag);

    if (iter == m_pLayers[iLevelIndex].end())
        return false;

    return (iter->second)->IsObjectAtPosition_GoodVerdict(vPosition);
}


CGameObject* CObject_Manager::ObjectAtPosition(_uint iLevelIndex, const _wstring& strLayerTag ,_float3 vPosition)
{
    auto iter = m_pLayers[iLevelIndex].find(strLayerTag);

    if (iter == m_pLayers[iLevelIndex].end())
        return false;

    return (iter->second)->ObjectAtPosition(vPosition);
}


CGameObject* CObject_Manager::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
    CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->Find_Object(iIndex);
}

CGameObject* CObject_Manager::Find_Player()
{
    CGameObject* pPlayer = Find_Object(1, TEXT("Layer_Player"), 0);
    if (nullptr == pPlayer)
        return nullptr;

    return pPlayer;
}

CGameObject* CObject_Manager::Find_Camera()
{
    CGameObject* pCamera = Find_Object(1, TEXT("Layer_Camera"), 0);
    if (nullptr == pCamera)
        return nullptr;

    return pCamera;
}


CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
    CObject_Manager* pInstance = new CObject_Manager;

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Manager::Free()
{
    __super::Free();

    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_pLayers[i])
            Safe_Release(Pair.second);
        m_pLayers[i].clear();
    }
    Safe_Delete_Array(m_pLayers);


    for (auto& Pair : m_Prototypes)
        Safe_Release(Pair.second);
    m_Prototypes.clear();
}
