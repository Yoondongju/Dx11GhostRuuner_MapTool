#include "stdafx.h"
#include "Player_Idle.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

CPlayer_Idle::CPlayer_Idle(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::IDLE , pOwner }
{

}

HRESULT CPlayer_Idle::Initialize()
{
    return S_OK;
}

HRESULT CPlayer_Idle::Start_State()
{
	

    return S_OK;
}

void CPlayer_Idle::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	
	Check_RunAndWalk();

	Check_Dash();

	//Check_HookUp();





}

void CPlayer_Idle::End_State()
{
	m_fAccTime = 0.f;
}


void CPlayer_Idle::Check_RunAndWalk()
{
	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD ||
		m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD ||
		m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD ||
		m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
		CPlayer::PLAYER_ANIMATIONID eID = CPlayer::PLAYER_ANIMATIONID::WALK;


		if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::HOLD)
			eID = CPlayer::PLAYER_ANIMATIONID::RUN;


		pFsm->Change_State(eID);
		pModel->SetUp_Animation(eID, true);
	}
}

void CPlayer_Idle::Check_Dash()
{
	_bool bWTap = m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::TAP;
	_bool bSTap = m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::TAP;
	_bool bATap = m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::TAP;
	_bool bDTap = m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::TAP;

	if (bWTap || bSTap || bATap || bDTap)
	{
		if (bWTap) {
			if (m_fLastKeyPressTimeW >= 0.f && (m_fAccTime - m_fLastKeyPressTimeW <= 0.2f))
			{
				// W Ű ���� Ŭ�� ����
				CState::STATE_DIR eDir = CState::STATE_DIR::FRONT;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_F, true);

				m_fAccTime = 0.f;
			}
			m_fLastKeyPressTimeW = m_fAccTime;
		}

		if (bSTap) {
			if (m_fLastKeyPressTimeS >= 0.f && (m_fAccTime - m_fLastKeyPressTimeS <= 0.2f))
			{
				// S Ű ���� Ŭ�� ����
				CState::STATE_DIR eDir = CState::STATE_DIR::BACK;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_B, true);

				m_fAccTime = 0.f;
			}
			m_fLastKeyPressTimeS = m_fAccTime;
		}

		if (bATap) {
			if (m_fLastKeyPressTimeA >= 0.f && (m_fAccTime - m_fLastKeyPressTimeA <= 0.2f))
			{
				// A Ű ���� Ŭ�� ����
				CState::STATE_DIR eDir = CState::STATE_DIR::LEFT;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_L, true);

				m_fAccTime = 0.f;
			}
			m_fLastKeyPressTimeA = m_fAccTime;
		}

		if (bDTap) {
			if (m_fLastKeyPressTimeD >= 0.f && (m_fAccTime - m_fLastKeyPressTimeD <= 0.2f))
			{
				// D Ű ���� Ŭ�� ����
				CState::STATE_DIR eDir = CState::STATE_DIR::RIGHT;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_R, true);

				m_fAccTime = 0.f;
			}
			m_fLastKeyPressTimeD = m_fAccTime;
		}
	}
}

void CPlayer_Idle::Check_HookUp()
{
	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false);
	}
}

CPlayer_Idle* CPlayer_Idle::Create(class CGameObject* pOwner)
{
	CPlayer_Idle* pInstance = new CPlayer_Idle(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Idle::Free()
{
	__super::Free();
}
