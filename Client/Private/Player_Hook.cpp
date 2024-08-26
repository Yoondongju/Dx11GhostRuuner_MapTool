#include "stdafx.h"
#include "Player_Hook.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

CPlayer_Hook::CPlayer_Hook(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::HOOK_UP , pOwner }
{

}

HRESULT CPlayer_Hook::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Hook::Start_State()
{


	return S_OK;
}

void CPlayer_Hook::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	m_pGameInstance->Find_Player()->Get_Transform()->Go_Straight(m_fAccTime * fTimeDelta * 15);



	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_DOWN, true);
	}


}

void CPlayer_Hook::End_State()
{
	m_fAccTime = 0.f;
}


CPlayer_Hook* CPlayer_Hook::Create(class CGameObject* pOwner)
{
	CPlayer_Hook* pInstance = new CPlayer_Hook(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Hook"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Hook::Free()
{
	__super::Free();
}
