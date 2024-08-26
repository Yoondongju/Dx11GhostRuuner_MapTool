#include "stdafx.h"
#include "Player.h"

#include "GameInstance.h"

#include "FreeCamera.h"

#include "Body_Player.h"
#include "Weapon_Player.h"

#include "Player_Idle.h"
#include "Player_Dash.h"
#include "Player_Block.h"
#include "Player_Attack.h"
#include "Player_Hook.h"
#include "Player_Climb.h"
#include "Player_Walk.h"
#include "Player_Run.h"

extern _bool g_PlayerFollowCamera;
extern _bool g_PlayerAnimationControler;

extern _bool g_FollowPlayer;

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice,pContext)
{

}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject(Prototype)
{

}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if(FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_State()))
        return E_FAIL;


    XMStoreFloat4x4(&m_RotationMatrix, XMMatrixIdentity());

    m_eCurAnimationID = PLAYER_ANIMATIONID::IDLE;
    m_pFsm->Set_State(PLAYER_ANIMATIONID::IDLE);

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
   


    for (auto& pPartObject : m_Parts)
        pPartObject->Priority_Update(fTimeDelta);
   
}

void CPlayer::Update(_float fTimeDelta)
{
    if (g_FollowPlayer)
    {
        CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera());

        _vector vCamPosition = pCamera->Get_Transform()->Get_State(CTransform::STATE_POSITION);

        _vector RotationOffset = XMVector3TransformCoord(XMLoadFloat4(&pCamera->Get_OffsetByPlayer()), XMLoadFloat4x4(&pCamera->Get_RotationMatrix()));

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition - RotationOffset);


        m_pTransformCom->Set_State(CTransform::STATE_RIGHT, pCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
        m_pTransformCom->Set_State(CTransform::STATE_UP, pCamera->Get_Transform()->Get_State(CTransform::STATE_UP));
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, pCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK));

    }

    //m_pFsm->Update(fTimeDelta);
    
    for (auto& pPartObject : m_Parts)
        pPartObject->Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_Parts)
        pPartObject->Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{



    return S_OK;
}

HRESULT CPlayer::Ready_Component()
{
    /* For.Com_Fsm */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Player_FSM"),
        TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsm), nullptr)))
        return E_FAIL;


   


    return S_OK;
}

HRESULT	CPlayer::Ready_State()
{
    if (FAILED(m_pFsm->Add_State(CPlayer_Idle::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Dash::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Block::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Attack::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Hook::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Climb::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Walk::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Run::Create(this))))
        return E_FAIL;


    return S_OK;
}




HRESULT CPlayer::Ready_PartObjects()
{
    /* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
    m_Parts.resize(PART_END);

    CBody_Player::BODY_DESC		BodyDesc{};
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.pOwner = this;
    BodyDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
        return E_FAIL;


    CWeapon_Player::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = dynamic_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Weapon_r");
    WeaponDesc.pOwner = this;
    WeaponDesc.InitWorldMatrix = XMMatrixIdentity();


    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon_Player"), &WeaponDesc)))
        return E_FAIL;


    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();



    Safe_Release(m_pFsm);
}
