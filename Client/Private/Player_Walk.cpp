#include "stdafx.h"
#include "Player_Walk.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

CPlayer_Walk::CPlayer_Walk(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::WALK ,pOwner }
{

}

HRESULT CPlayer_Walk::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Walk::Start_State()
{

	return S_OK;
}

void CPlayer_Walk::Update(_float fTimeDelta)
{	
	if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::HOLD)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::RUN);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::RUN, true);
	}
	


	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
	}
}

void CPlayer_Walk::End_State()
{

}


CPlayer_Walk* CPlayer_Walk::Create(class CGameObject* pOwner)
{
	CPlayer_Walk* pInstance = new CPlayer_Walk(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Walk::Free()
{
	__super::Free();
}
