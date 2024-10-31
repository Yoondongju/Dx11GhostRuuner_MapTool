#include "stdafx.h"
#include "..\Public\Loader.h"

#include "BackGround.h"
#include "GameInstance.h"
#include "Camera.h"

#include "Fsm.h"
#include "Navigation.h"

#include "Terrain.h"
#include "FreeCamera.h"


#include "Player.h"
#include "Body_Player.h"
#include "Weapon_Player.h"

#include "Sky.h"


#include "Spider.h"

#include "Elite.h"
#include "Weapon_Elite.h"
#include "Weapon_Hel.h"

#include "Jetpack.h"
#include "Weapon_Jetpack.h"
#include "BackPack_Jetpack.h"

#include "Mira.h"

#include "Pistol.h"
#include "Weapon_Pistol.h"

#include "Sniper.h"
#include "Weapon_Sniper.h"

#include "Hel.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}


HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_MAPTOOL:
		hr = Ready_Resources_For_MapToolLevel();
		break;
	case LEVEL_LOGO:
		hr = Ready_Resources_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_MapToolLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Ghostrunner/TileTest.png"), 1))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	if (FAILED(Ready_Fsm()))
		return E_FAIL;

	if (FAILED(Ready_Model()))
		return E_FAIL;

	if (FAILED(Ready_VIBuffer()))
		return E_FAIL;

	if (FAILED(Ready_Navigation()))
		return E_FAIL;

	if (FAILED(Ready_Shader()))
		return E_FAIL;

	if (FAILED(Ready_ObjectPrototype()))
		return E_FAIL;

	if (FAILED(Ready_Other_Model()))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Fsm()
{
	lstrcpy(m_szLoadingText, TEXT("FSM을(를) 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Player_FSM"),
		CFsm::Create(m_pDevice, m_pContext, CPlayer::PLAYER_ANIMATIONID::PLAYER_ANIMATION_END))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Model()
{	
	if (FAILED(Ready_Map_Model()))
		return E_FAIL;

	if (FAILED(Ready_Player_Model()))
		return E_FAIL;

	if (FAILED(Ready_Monster_Model()))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Ready_Map_Model()
{
	// if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Sky"),
	// 	CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Sky/sky.fbx"))))
	// 	return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("KitObject_Model을(를) 로딩중입니다."));

	/* For. Prototype_Component_Kit_Model*/
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Kit0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Kit/kit0.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Kit1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Kit/kit1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Kit2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Kit/kit2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Kit3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Kit/kit3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Kit4"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Kit/kit4.fbx", PreTransformMatrix))))
		return E_FAIL;
	// 5개


	lstrcpy(m_szLoadingText, TEXT("DebrisObject_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Debris_Model*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Debris0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Debris/Debris0.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Debris1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Debris/Debris1.fbx", PreTransformMatrix))))
		return E_FAIL;
	// 2개


	lstrcpy(m_szLoadingText, TEXT("BarricadeObject_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Barricade*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Barricade0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Barricade/Barricade.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Barricade1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Barricade/Barricade2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Barricade2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Barricade/Barricade3.fbx", PreTransformMatrix))))
		return E_FAIL;
	// 3개


	lstrcpy(m_szLoadingText, TEXT("BillboardObject_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Billboard*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard4"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard5"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard6"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/7.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard7"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/8.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard8"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/9.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard9"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/10.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard10"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/11.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Billboard11"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Billboard/12.fbx", PreTransformMatrix))))
		return E_FAIL;
	// 12개



	lstrcpy(m_szLoadingText, TEXT("Boss_Fight1Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Boss_Fight1*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight4"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight5"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight6"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/7.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight7"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/8.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight8"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/9.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight9"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/10.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight10"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/11.fbx", PreTransformMatrix))))
		return E_FAIL;		 

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight11"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/12.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight12"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/13.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight13"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/14.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight14"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/15.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight15"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/16.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight16"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/17.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight17"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/18.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight18"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/19.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight19"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/20.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight20"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight1/21.fbx", PreTransformMatrix))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("Boss_Fight3Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_boss_fight3*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight3_0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight3/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Fight3_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/boss_fight3/2.fbx", PreTransformMatrix))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("CatWalk Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_CatWalk*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CatWalk0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CatWalk/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CatWalk1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CatWalk/3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CatWalk2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CatWalk/4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CatWalk3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CatWalk/5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CatWalk4"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CatWalk/6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CatWalk5"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CatWalk/7.fbx", PreTransformMatrix))))
		return E_FAIL;

	
	lstrcpy(m_szLoadingText, TEXT("Crane Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Crane*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Crane0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Crane/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Crane1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Crane/2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Crane2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Crane/3.fbx", PreTransformMatrix))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("CyberCity Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_CyberCity*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CyberCity0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CyberCity/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CyberCity1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CyberCity/2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CyberCity2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CyberCity/3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_CyberCity3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/CyberCity/4.fbx", PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Deco Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Deco*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Deco0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/deco/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Deco1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/deco/2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Deco2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/deco/3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Deco3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/deco/4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Deco4"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/deco/5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Deco5"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/deco/6.fbx", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Fan Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Fan*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Fan0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Fan/Fan.fbx", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Industrial_Elevator Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Industrial_Elevator*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Industrial_Elevator0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Industrial_Elevator/1.fbx", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Lamp Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Lamp*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Lamp0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Lamp/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Lamp1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Lamp/2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Lamp2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Lamp/3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Lamp3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Lamp/4.fbx", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Pipe Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Pipe*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pipe0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Pipe/Pipe.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pipe1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Pipe/Pipe2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pipe2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Pipe/Pipe3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pipe3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Pipe/Pipe4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pipe4"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Pipe/Pipe5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pipe5"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Pipe/Pipe6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pipe6"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Pipe/Pipe7.fbx", PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Platform Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Platform*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Platform0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Platform/door1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Platform1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Platform/doorframe.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Platform2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Platform/OutDoor.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Platform3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Platform/platform.fbx", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("RotationFan Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_RotationFan*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_RotationFan0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/RotationFan/1.fbx", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Train Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Train*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Train0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Train/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Train1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Train/2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Train2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Train/3.fbx", PreTransformMatrix))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("BodyBag Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Train*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_BodyBag0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/BodyBag/1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_BodyBag1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/BodyBag/2.fbx", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Climb Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Train*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_ClimbObject0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/ClimbObject/1.fbx", PreTransformMatrix))))
		return E_FAIL;







	lstrcpy(m_szLoadingText, TEXT("Sign Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Sign_Model*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Sign0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Sign/sign0.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Sign1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Sign/sign1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Sign2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Sign/sign2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Sign3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Sign/sign3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Sign4"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Sign/sign4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Sign5"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Sign/sign5.fbx", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Wall Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Wall_Model*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Wall0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Wall/wall0.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Wall1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Wall/wall1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Wall2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Wall/wall2.fbx", PreTransformMatrix))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("Russian_Sign Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Russian_Sign_Model*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign0.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign4"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign5"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign6"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign7"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign7.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign8"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign8.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign9"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign9.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Russian_Sign10"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Sign/Russian_sign/russian_sign10.fbx", PreTransformMatrix))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("Building Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Level_City_Model*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_City"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Level_City/City.fbx", PreTransformMatrix))))
		return E_FAIL;


	///* For. Prototype_Component_Level_City_Building_Model*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Building0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Level_City/MapObject/Building/Building0.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Building1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Level_City/MapObject/Building/Building1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Building2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Level_City/MapObject/Building/Building2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Building3"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Level_City/MapObject/Building/Building3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Building4"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Level_City/MapObject/Building/Building4.fbx", PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Wire Object_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Level_City_Wire_Model*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Wire0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Level_City/MapObject/Wire/Wire0.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Wire1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Level_City/MapObject/Wire/Wire1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Wire2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Level_City/MapObject/Wire/Wire2.fbx", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Player_Model()
{
	lstrcpy(m_szLoadingText, TEXT("Player_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Model_Player*/
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Player/Player0908.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Player_Weapon"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Player/Weapon/Weapon0.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Player_Weapon2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Player/Weapon/Weapon1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Player_Katana"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Player/Weapon/katana.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Player_Shuriken"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Player/Weapon/Shuriken.fbx", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Monster_Model()
{
	lstrcpy(m_szLoadingText, TEXT("Monster_Model을(를) 로딩중입니다."));

	///* For. Prototype_Component_Model_Monster_Spider*/
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Spider"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Monster/Spider.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Hel"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Monster/Hel/BossHel.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Elite"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Monster/Elite/Elite.fbx", PreTransformMatrix))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Jetpack"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Monster/Jetpack/Jetpack.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Jetpack_Back"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Monster/Jetpack/Jetpack_Back.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Jetpack_Roket"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Monster/Jetpack/Jetpack_roket.fbx", PreTransformMatrix))))
		return E_FAIL;



	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Mira"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Monster/mira/mira.fbx", PreTransformMatrix))))
		return E_FAIL;




	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pistol"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Monster/pistol/pistol.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pistol_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Monster/pistol/pistol_gun.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Sniper"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Monster/sniper/sniper.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pistol_Sniper_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Monster/sniper/sniper_gun.fbx", PreTransformMatrix))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLoader::Ready_Other_Model()
{
	lstrcpy(m_szLoadingText, TEXT("Other_Model을(를) 로딩중입니다."));


	///* For. Prototype_Component_Model_Monster_Spider*/
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);


	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_MapModel"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/MapData/Map_Stage1.fbx", PreTransformMatrix))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_MapModel"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/MapData/CyberCityset1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Sky"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Sky/sky1.fbx", PreTransformMatrix))))
		return E_FAIL;





	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Blood1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Blood/Blood1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Piece"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Piece/Piece.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Electric"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Electric.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_BigElectric"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/BigElectric.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.06f, 0.06f, 0.06f) * XMMatrixRotationX(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Bullet"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Bullet/Bullet.fbx", PreTransformMatrix))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Blood2"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Blood/Blood2.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Blood3"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Blood/Blood3.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Blood4"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Blood/Blood4.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Blood5"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Blood/Blood5.fbx", PreTransformMatrix))))
	//	return E_FAIL;


	


	return S_OK;
}



HRESULT CLoader::Ready_VIBuffer()
{
	lstrcpy(m_szLoadingText, TEXT("터레인 버퍼(를) 로딩중입니다."));
	///* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 20, 20, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Navigation()
{
	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Ready_Shader()
{
	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));
	///* For. Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxMeshTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxMeshTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshTex.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxAnimModelTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	///* For. Prototype_Component_Shader_VtxCube*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_ObjectPrototype()
{
	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Player"),
		CWeapon_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;





	// 실제 몬스터 클래스 타입별 객체원형 
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hel"),
		CHel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Hel"),
		CWeapon_Hel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spider"),
		CSpider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Elite"),
		CElite::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Elite"),
		CWeapon_Elite::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Jetpack"),
		CJetpack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Jetpack"),
		CWeapon_Jetpack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackPack_Jetpack"),
		CBackPack_Jetpack::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mira"),
		CMira::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pistol"),
		CPistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Pistol"),
		CWeapon_Pistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sniper"),
		CSniper::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Sniper"),
		CWeapon_Sniper::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}





HRESULT CLoader::Ready_Resources_For_LogoLevel()
{

	//lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	///* For. Prototype_Component_Texture_BackGround */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	//	return E_FAIL;
	//
	//lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	//
	//
	//lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));
	//
	//
	//lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	///* For. Prototype_GameObject_BackGround */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
	//	CBackGround::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//
	//lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	//
	//m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
