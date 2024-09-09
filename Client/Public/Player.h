#pragma once


#include "Client_Defines.h"
#include "ContainerObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CFsm;
class CNavigation;
END


BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum PARTID { PART_BODY, PART_WEAPON, PART_END };
	enum PLAYER_ANIMATIONID
	{
		CLIMB,
		DASH_B,
		DASH_L,
		DASH_R,
		BLOCK_R1,
		BLOCK_R2,
		BLOCK_R3,
		HOOK_UP,

		BLOCK_L1,
		BLOCK_L2,
		BLOCK_L3,

		ATTACK_R1,
		ATTACK_R2,
		ATTACK_R3,

		ATTACK_L1,
		ATTACK_L2,
		ATTACK_L3,

		JUMP_IDLE,

		DASH_F,
		IDLE,

		JUMP_START,
		JUMP_LOOP,
		JUMP_END,

		RUN,
		RUN_WALL_L,
		RUN_WALL_R,

		SLIDE,
		WALK,

		FORCE_PUSH,

		FURR_AIM_LOOP,				// 정신집중
		FURR_AIM_TO_IDLE,
		FURR_IDLE_TO_AIM,
		
		RIFT_PICKUP,

		FURR_AIM_TRICUT,

		HACK_IDLE_TO_AIM_LOOP,

		NAMI_AIM_ATTACK_TO_IDLE,

		HACK_AIM_TO_IDLE,			// 마인드컨트롤 해제하는거? 왼쪽손으로 바스라지는 느낌의 애니

		DUMMY,

		NAMI_IDLE_TO_AIM,			// 왼손 쫙 펴는거 

		DUMMY2,



		SH_ATTACK,

		SH_DASH_B,
		SH_DASH_F,
		SH_DASH_L,
		SH_DASH_R,

		SH_IDLE,
		SH_RUN,
		SH_WALK,
		SH_RUN_WALL_L,
		SH_RUN_WALL_R,
		SH_KAT_TO_SHUR,				// 카타나에서 수리켄
		SH_SHUR_TO_KAT,				// 수리켄에서 카타나

		HACK_AIM_LOOP,

		TIME_STOP,					// 시간정지

		FURR_AIM_TO_DASH,			// 대쉬하면서 한번 베기
		FURR_DASH_TRICUT,			// 대쉬하면서 여러번 베기

		DEATH_1,
		DEATH_2,
		
		DRAW_SWORD,	
		LOOKAT_HAND,
		SPIN_A,
		SPIN_B,


		PLAYER_ANIMATION_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	class CFsm*			Get_Fsm() override { return m_pFsm; }
	PLAYER_ANIMATIONID	Get_CurAnimationID() { return m_eCurAnimationID; }

	const _float4x4& Get_RotationMatrix() { return m_RotationMatrix; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CFsm*					m_pFsm = { nullptr };
	
	PLAYER_ANIMATIONID		m_eCurAnimationID = { PLAYER_ANIMATION_END };

	POINT					m_ptOldMousePos = {};
	_float4x4				m_RotationMatrix = {};



private:
	HRESULT		Ready_Component();
	HRESULT		Ready_PartObjects();

	HRESULT		Ready_State();
	





public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END