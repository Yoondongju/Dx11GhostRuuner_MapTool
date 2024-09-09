#include "stdafx.h"
#include "..\Public\FreeCamera.h"

#include "GameInstance.h"

#include "Player.h"



extern _bool g_FollowPlayer;

CFreeCamera::CFreeCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CFreeCamera::CFreeCamera(const CFreeCamera& Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CFreeCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFreeCamera::Initialize(void* pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

	m_fSensor = pDesc->fSensor;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	/* For.Com_Navigation */				// 내비 게이션을 만들게아니라 어떤 모델의 네비게이션을 참조하는지 체크해야할것같음
	CNavigation::NAVIGATION_DESC			NaviDesc{};
	NaviDesc.iCurrentIndex = 0;
	
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pCurNavigationCom), &NaviDesc)))
		return E_FAIL;


	//XMStoreFloat4(&m_OffsetByPlayer, XMVectorSet(0.f, 15.42215824f, 2.0040216f, 1.f));
	XMStoreFloat4x4(&m_RotationMatrix, XMMatrixIdentity());

	return S_OK;
}


void CFreeCamera::Set_PickedObj(CGameObject* pGameObj)
{
	m_pPickedObj = pGameObj;
}


void CFreeCamera::Priority_Update(_float fTimeDelta)
{
	CNavigation* pNavigation = nullptr;

	//if (g_FollowPlayer)
	//{
	//	// 네비를 달고 있는 레이어중에서 탐색을 한다.
	//	
	//	list<CGameObject*>& Terrains = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_BackGround_Terrain");
	//	for (auto& Terrain : Terrains)
	//	{
	//		_vector vCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//
	//		_matrix InverseWorldMatrix = {};
	//		InverseWorldMatrix = XMMatrixInverse(nullptr, Terrain->Get_Transform()->Get_WorldMatrix());
	//
	//		vCamPos = XMVector3TransformCoord(vCamPos, InverseWorldMatrix);
	//
	//
	//		_float3 CamLocalPos = {};
	//		XMStoreFloat3(&CamLocalPos, vCamPos);
	//
	//		
	//		if (Terrain->Get_Navigation()->IsInNavigationRange_About_Local(CamLocalPos))		// 일단 지형만 ..
	//		{				
	//			m_pCurNavigationCom->Set_OwnerWorldMatrix(Terrain->Get_Transform()->Get_WorldMatrix_Ptr());
	//
	//			break;
	//		}
	//			
	//	}
	//
	//
	//	
	//
	//	pNavigation = m_pCurNavigationCom;
	//}

	XMStoreFloat4(&m_OffsetByPlayer, XMVectorSet(0.f, 15.42215824f, 8.0040216f, 1.f));

	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
		m_pTransformCom->Go_Straight(fTimeDelta, pNavigation);
	if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
		m_pTransformCom->Go_Backward(fTimeDelta, pNavigation);
	if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
		m_pTransformCom->Go_Left(fTimeDelta, pNavigation);
	if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
		m_pTransformCom->Go_Right(fTimeDelta, pNavigation);


	POINT ptMousePos = {};
	GetCursorPos(&ptMousePos);

	if (g_hWnd == GetFocus() && m_pGameInstance->Get_KeyState(KEY::RBUTTON) == KEY_STATE::HOLD)
	{
		_long	MouseMove = {};
		static CGameObject* pPlayer = m_pGameInstance->Find_Player();

		if (MouseMove = ptMousePos.x - m_ptOldMousePos.x)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.07f, &m_RotationMatrix);
		}

		if (MouseMove = ptMousePos.y - m_ptOldMousePos.y)
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.07f, &m_RotationMatrix);
		}

	}

	m_ptOldMousePos = ptMousePos;

	__super::Priority_Update(fTimeDelta);
}

void CFreeCamera::Update(_float fTimeDelta)
{
	
	if (nullptr != m_pPickedObj)
	{
		MoveToPickedObj(fTimeDelta);
	}

}

void CFreeCamera::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

}

HRESULT CFreeCamera::Render()
{

#ifdef _DEBUG
	m_pCurNavigationCom->Render();
#endif

	return S_OK;
}

void CFreeCamera::MoveToPickedObj(_float fTimeDelta)
{
	_float3 vAbsDiff = {};
	XMStoreFloat3(&vAbsDiff, XMVectorAbs(XMVectorSubtract(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION), m_pPickedObj->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION))));
	
	_float fDistance = {};

	XMStoreFloat(&fDistance , XMVector3Length(XMLoadFloat3(&vAbsDiff)));
	 
	if ( 20.f < fDistance)
	{		
		m_pTransformCom->LookAt(m_pPickedObj->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION));
		m_pTransformCom->Go_Straight_TargetObject(fDistance * 0.1f * fTimeDelta);
	}
	else
	{
		m_fAccTime = 0.f;
		m_pPickedObj = nullptr;
	}

}

CFreeCamera* CFreeCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFreeCamera* pInstance = new CFreeCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFreeCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFreeCamera::Clone(void* pArg)
{
	CFreeCamera* pInstance = new CFreeCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFreeCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFreeCamera::Free()
{
	__super::Free();

	Safe_Release(m_pCurNavigationCom);
	Safe_Release(m_pPickedObj);
}
