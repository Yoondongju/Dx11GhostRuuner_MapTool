#include "../Public/GameObject.h"
#include "../Public/GameInstance.h"

#include "../Public/Texture.h"

CGameObject::CGameObject(ID3D11Device*  pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext {pContext}
	, m_pGameInstance {CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pDevice { Prototype.m_pDevice}
	, m_pContext {Prototype.m_pContext}
	, m_pGameInstance   { Prototype.m_pGameInstance }
{
	Safe_AddRef(m_pGameInstance);

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

}


HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	if (nullptr != pDesc)
	{
		m_iModelNum = pDesc->iModelNum;             // 몇번째 모델을 사용할거니 ? 같은 클래스의 오브젝트더라도 모델이 다를수 있지?
		m_iModelListType = pDesc->iModelListType;

		m_iObjectType = pDesc->iObjectType;		// 데코,스태틱,다이나믹?
	}
	

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, pDesc);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	/* 다른 객체가 내 컴포넌트를 검색할 수 있도록 맵에다가 컴포넌트를 모아둔다. */
	m_Components.emplace(g_strTransformTag, m_pTransformCom);
	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
	if (m_strLayerName == L"Layer_PreView_Object")
	{		
		// 일단 지형의 높이가 0이니까 임시로 0
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_pGameInstance->Get_PickingBackGround_JustPos(0.f)));
	}
}


void CGameObject::Late_Update(_float fTimeDelta)
{


}

HRESULT CGameObject::Render()
{
	return S_OK;
}

CComponent* CGameObject::Find_Component(const _wstring& strComponentTag)
{
	auto iter = m_Components.find(strComponentTag);
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

CComponent* CGameObject::Find_Component(COMPONENT_TYPE eComponentTag)
{	
	for (auto iter = m_Components.begin(); iter != m_Components.end(); ++iter)
	{
		if (eComponentTag == iter->second->Get_ComponentType())
			return iter->second;
	}

	return nullptr;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _wstring strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	CComponent* pPreComponent = Find_Component(strComponentTag);
	if (nullptr != pPreComponent)
	{
		Safe_Release(pPreComponent);
		m_Components.erase(strComponentTag);		
	}

	CComponent* pComoponent =  m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);

	m_Components.emplace(strComponentTag, pComoponent);
	
	*ppOut = pComoponent;
	Safe_AddRef(pComoponent);

	return S_OK;
}

CGameObject* CGameObject::Clone(void* pArg)
{
	return nullptr;
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
