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

	/* For. Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pDevice , m_pContext, TEXT("../Bin/Ghostrunner/Sky.dds"), 1))))
	//	return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("FSM을(를) 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Player_FSM"),
		CFsm::Create(m_pDevice, m_pContext , CPlayer::PLAYER_ANIMATIONID::PLAYER_ANIMATION_END))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	///* For. Prototype_Component_SKY*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pDevice,m_pContext))))
	//	return E_FAIL;



	/* For. Prototype_Component_Kit_Model*/
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Kit0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Kit/kit0.fbx" ,PreTransformMatrix))))
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
	
	
	///* For. Prototype_Component_Debris_Model*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Debris0"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Debris/Debris0.fbx", PreTransformMatrix))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Debris1"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Map/Prop/Debris/Debris1.fbx", PreTransformMatrix))))
		return E_FAIL;
	
	
	
	
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



	///* For. Prototype_Component_Model_Player*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Player/Player_final.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Player_Weapon"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Player/Weapon/Weapon0.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Player_Weapon2"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, "../Bin/Ghostrunner/Player/Weapon/Weapon1.fbx", PreTransformMatrix))))
		return E_FAIL;
	



	///* For. Prototype_Component_Model_Monster_Spider*/
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Spider"),
		CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_ANIM, "../Bin/Ghostrunner/Monster/Spider.fbx", PreTransformMatrix))))
		return E_FAIL;

	


	
	// =====================================================================  //



	
	///* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 20, 20, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;

	
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




	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));





	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
	//	CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

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

	

	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice,m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

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
