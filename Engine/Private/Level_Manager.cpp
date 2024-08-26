#include "../Public/Level_Manager.h"
#include "../Public/Level.h"
#include "../Public/GameInstance.h"

CLevel_Manager::CLevel_Manager()
	:m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Initialize()
{

	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	if (nullptr != m_pCurrentLevel)
		m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr != m_pCurrentLevel)
		m_pCurrentLevel->Render();

	return S_OK;
}

HRESULT CLevel_Manager::ImGui_Grid_Obejct_Gizumo_Render()
{
	if (nullptr != m_pCurrentLevel && m_iLevelIndex == 1)	
		m_pCurrentLevel->Render();

	return S_OK;
}

HRESULT CLevel_Manager::Change_Level(_uint iLevelIndex , CLevel* pNextLevel)
{
	m_pGameInstance->Clear(m_iLevelIndex);  // 레벨이 넘어갈때 이전레벨에서 사용하던 객체를 삭제해줄 필요가 있다.
	 
	Safe_Release(m_pCurrentLevel);          // 씬 바뀔때 기존씬을 걍 삭제한다.. 맞나?


    m_pCurrentLevel = pNextLevel;
	m_iLevelIndex = iLevelIndex;

	return S_OK;
}

CLevel_Manager* CLevel_Manager::Create()
{
	CLevel_Manager* pInstance = new CLevel_Manager;
	
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCurrentLevel);
}
