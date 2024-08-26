#include "stdafx.h"
#include "Player_Dash.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

CPlayer_Dash::CPlayer_Dash(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::DASH_F , pOwner }		// 이거 고민이다.. 일단그냥 제일 최상위의 애니메이션 인덱스에 넣어야할듯?
{

}

HRESULT CPlayer_Dash::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Dash::Start_State()
{

	return S_OK;
}

void CPlayer_Dash::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	if (m_fAccTime >= 0.5f)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
 

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);

		m_fAccTime = 0.f;
	}

	switch (m_eDir)
	{
	case Engine::CState::FRONT:
		m_pGameInstance->Find_Player()->Get_Transform()->Go_Straight(m_fAccTime * fTimeDelta * 10);
		break;
	case Engine::CState::BACK:
		m_pGameInstance->Find_Player()->Get_Transform()->Go_Backward(m_fAccTime * fTimeDelta * 10);
		break;
	case Engine::CState::LEFT:
		m_pGameInstance->Find_Player()->Get_Transform()->Go_Left(m_fAccTime * fTimeDelta * 10);
		break;
	case Engine::CState::RIGHT:
		m_pGameInstance->Find_Player()->Get_Transform()->Go_Right(m_fAccTime * fTimeDelta * 10);
		break;
	case Engine::CState::STATE_DIR_END:
		break;
	default:
		break;
	}	

}

void CPlayer_Dash::End_State()
{
	m_fAccTime = 0.f;
}


CPlayer_Dash* CPlayer_Dash::Create(class CGameObject* pOwner)
{
	CPlayer_Dash* pInstance = new CPlayer_Dash(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Dash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Dash::Free()
{
	__super::Free();
}
