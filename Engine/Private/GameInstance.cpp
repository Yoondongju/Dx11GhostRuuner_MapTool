#include "../Public/GameInstance.h"
#include "../Public/Graphic_Device.h"

#include "../Public/Level_Manager.h"
#include "../Public/Object_Manager.h"
#include "../Public/Timer_Manager.h"

/* 우리가 추가한 기능*/
#include "../Public/Key_Manager.h"
#include "../Public/CEvent_Manager.h"
#include "../Public/UI_Manager.h"
#include "../Public/PickingManager.h"
#include "../Public/PhysXManager.h"

#include "../Public/Layer.h"

#include "../Public/GameObject.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

POINT CGameInstance::Get_MosePos()
{
	POINT		ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	return ptMouse;
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc , _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	m_pDevice = CGraphic_Device::Create(EngineDesc, ppDevice, ppContext);
	if (nullptr == m_pDevice)
		return E_FAIL;

	m_hWnd = EngineDesc.hWnd;

	

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;
	


	m_pEvent_Manager = CEvent_Manager::Create();
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;


	m_pKey_Manager = CKey_Manager::Create(m_hWnd);
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pUI_Manager = CUI_Manager::Create();
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	m_pPicking_Manager = CPickingManager::Create(*ppDevice, *ppContext, m_hWnd);
	if (nullptr == m_pPicking_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return S_OK;

	//m_pPhysX_Manager = CPhysXManager::Create();
	//if (nullptr == m_pPhysX_Manager)
	//	return S_OK;



	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	//키 값은 항상 먼저 update
	m_pKey_Manager->Update();

	m_pPipeLine->Update();

	m_pPicking_Manager->Update();

	m_pPhysX_Manager->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);
	m_pObject_Manager->Update(fTimeDelta);
	m_pObject_Manager->Late_Update(fTimeDelta);


	m_pLevel_Manager->Update(fTimeDelta);

	//UIManager는 항상 Render 되기 전에 호출
	m_pUI_Manager->Update();
}

void CGameInstance::FinalUpdate_Engine()
{
	m_pEvent_Manager->update();
}

HRESULT CGameInstance::Draw_Engine()
{
	m_pRenderer->Draw();
	m_pLevel_Manager->Render();

	return S_OK;
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	m_pObject_Manager->Clear(iLevelIndex);

	return S_OK;
}

void CGameInstance::Render_Begin()
{
	m_pDevice->Render_Begin();
}

void CGameInstance::Render_End(HWND hWnd)
{
	m_pDevice->Render_End(hWnd);
}

void CGameInstance::RenderText()
{
	m_pDevice->RenderText();
}


HRESULT CGameInstance::Change_Level(_uint iLevelIndx , CLevel* pNextLevel)
{
	//CEvent_Manager::EVENT eEvent{};
	//
	//eEvent.eEven = CEvent_Manager::EVENT_TYPE::CHANGE_LEVEL;
	//eEvent.lParam = (DWORD_PTR)iLevelIndx;
	//eEvent.wParam = (DWORD_PTR)pNextLevel;
	//
	//m_pEvent_Manager->AddEvent(eEvent);
	//
	//return S_OK;
	return m_pLevel_Manager->Change_Level(iLevelIndx,pNextLevel);
}

CLevel* CGameInstance::GetCurLevel()
{
	return m_pLevel_Manager->GetCurLevel();
}

HRESULT CGameInstance::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}


HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
	//CEvent_Manager::EVENT eEvent{};
	//eEvent.eEven = CEvent_Manager::EVENT_TYPE::CREATE_OBJECT;
	//
	//
	//CEvent_Manager::CLONE_DESC eCloneDesc{};
	//eCloneDesc.iLevelIndex = iLevelIndex;
	//eCloneDesc.strLayerTag = strLayerTag;
	//eCloneDesc.strPrototypeTag = strPrototypeTag;
	//eCloneDesc.pArg = pArg;
	//
	//eEvent.CloneDesc = eCloneDesc;
	//
	//m_pEvent_Manager->AddEvent(eEvent);   // << push_back 이기에 원본을 복사해서 넣기에 eEvent가 지역변수여도 상관없다
	//
	//return S_OK;
  	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject(const _wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

CComponent* CGameInstance::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CGameObject* CGameInstance::Find_Player()
{
	return m_pObject_Manager->Find_Player();
}

CGameObject* CGameInstance::Find_Camera()
{
	return m_pObject_Manager->Find_Camera();
}



CGameObject* CGameInstance::IsObjectAtPosition(_uint iLevelIndex, const _wstring& strLayerTag, _float3 vPosition)
{
	return m_pObject_Manager->IsObjectAtPosition(iLevelIndex,strLayerTag,vPosition);
}

CGameObject* CGameInstance::IsObjectAtPosition_GoodVerdict(_uint iLevelIndex, const _wstring& strLayerTag, _float3 vPosition)
{
	return m_pObject_Manager->IsObjectAtPosition_GoodVerdict(iLevelIndex, strLayerTag, vPosition);
}

CGameObject* CGameInstance::ObjectAtPosition(_uint iLevelIndex, const _wstring& strLayerTag, _float3 vPosition)
{
	return m_pObject_Manager->ObjectAtPosition(iLevelIndex,strLayerTag,vPosition);
}


list<class CGameObject*>& CGameInstance::Get_GameObjects(_uint iLevelIndx, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_GameObjects(iLevelIndx,strLayerTag);
}



HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

CComponent* CGameInstance::Get_Prototype(_uint iNumLevelIndex, const _wstring& strPrototypeTag)
{
	return m_pComponent_Manager->Get_Prototype(iNumLevelIndex,strPrototypeTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Update_Timer(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Update(strTimerTag);
}

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject* pRenderObject)
{
	if (FAILED(m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject)))
		return E_FAIL;

	return S_OK;
}

list<class CGameObject*>& CGameInstance::Get_RenderList(CRenderer::RENDERGROUP eGroup)
{
	return m_pRenderer->Get_RenderList(eGroup);
}


KEY_STATE CGameInstance::Get_KeyState(KEY _eKey)
{
	return m_pKey_Manager->Get_KeyState(_eKey);
}

HRESULT CGameInstance::Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize, void* pArg)
{
	return E_NOTIMPL;
}

HRESULT CGameInstance::Event_DestoryObject(CGameObject* pGameObject)
{
	CEvent_Manager::EVENT tEvent = {};
	tEvent.eEven = CEvent_Manager::EVENT_TYPE::DELETE_OBJECT;
	tEvent.lParam = (DWORD_PTR)pGameObject;


	m_pEvent_Manager->AddEvent(tEvent);
	return E_NOTIMPL;
}

_float3 CGameInstance::Get_PickingObject_CenterPos(_uint iLevelIndex , const _wstring& strLayerTag)
{
	// 클릭 했을때만 업데이트 돌자
	CLayer* pLayer = m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		return _float3(0.f,0.f,0.f);
	}
	
	 // 현재 레벨의 Picking 체크할 레이어의 모든 오브젝트를 순회해야한다.
	
	return m_pPicking_Manager->Get_PickingObject_CenterPos(pLayer);
}

_float3 CGameInstance::Get_PickingObject_JustPos(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		return _float3(0.f, 0.f, 0.f);
	}

	// 현재 레벨의 Picking 체크할 레이어의 모든 오브젝트를 순회해야한다.

	return m_pPicking_Manager->Get_PickingObject_JustPos(pLayer);
}

_float3 CGameInstance::Get_PickingBackGround_JustPos(_float fTargetY)
{
	return m_pPicking_Manager->Get_PickingBackGround_JustPos(fTargetY);
}

void CGameInstance::Transform_MouseRay_ToLocalSpace(const _float4x4& WorldMatrix)
{
	return m_pPicking_Manager->Transform_ToLocalSpace(WorldMatrix);
}

_bool CGameInstance::isPicked_InWorldSpace(const BoundingBox& MeshBoundingBox, _float3* pOut)
{
	return m_pPicking_Manager->isPicked_InWorldSpace(MeshBoundingBox, pOut);
}

_bool CGameInstance::isPicked_InLocalSpace(const BoundingBox& MeshBoundingBox, _float3* pOut)
{
	return m_pPicking_Manager->isPicked_InLocalSpace(MeshBoundingBox, pOut);
}

_bool CGameInstance::isPicked_InLocalSpace(const _vector& vPointA, const _vector& vPointB, const _vector& vPointC, _vector* pOut)
{
	return m_pPicking_Manager->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut);
}


void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}





void CGameInstance::Delete(_uint iLevelIndex, CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pObj)
{
	CEvent_Manager::EVENT eEvent{};
	eEvent.eEven = CEvent_Manager::EVENT_TYPE::DELETE_OBJECT;
	eEvent.lParam = (DWORD_PTR)pObj;


	pObj->Set_Dead(true);

	list<CGameObject*>& RenderObjectList = m_pRenderer->Get_RenderList(eRenderGroup);
	for (auto iter = RenderObjectList.begin(); iter != RenderObjectList.end();)
	{
		if (*iter == pObj) // 단순 객체의 주소 비교
		{
			iter = RenderObjectList.erase(iter);		// 객체를 삭제 하진않고 리스트에서 빼주기만 할거야
		}
		else
			++iter;
	}
	
	list<CGameObject*>& GameObjectList = m_pObject_Manager->Get_GameObjects(iLevelIndex, pObj->Get_LayerName());
	for (auto iter = GameObjectList.begin(); iter != GameObjectList.end();)
	{
		if (*iter == pObj) // 단순 객체의 주소 비교
		{
			iter = GameObjectList.erase(iter);		// 객체를 삭제 하진않고 리스트에서 빼주기만 할거야
		}
		else
			++iter;
	}


	m_pEvent_Manager->AddEvent(eEvent);
}

PxPhysics* CGameInstance::Get_Physics()
{
	return m_pPhysX_Manager->Get_Physics();
}

PxScene* CGameInstance::Get_Scene()
{
	return m_pPhysX_Manager->Get_Scene();
}

PxMaterial* CGameInstance::Get_Material()
{
	return m_pPhysX_Manager->Get_Material();
}

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pPhysX_Manager);

	Safe_Release(m_pPipeLine);
	Safe_Release(m_pPicking_Manager);
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pDevice);


	CGameInstance::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	__super::Free();
	
}


