#pragma once

#include "Base.h"


BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	class CLevel* GetCurLevel() { return m_pCurrentLevel; }
	_uint		  GetCurLevelIndex() { return m_iLevelIndex; }

public:
	 HRESULT	Initialize();
	 void		Update(_float fTimeDelta);
	 HRESULT	Render();
	 HRESULT    ImGui_Grid_Obejct_Gizumo_Render();

	 HRESULT	Change_Level(_uint iLevelIndex ,class CLevel* pNextLevel);	


private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	_uint					m_iLevelIndex = { };

public:
	static CLevel_Manager* Create();
	virtual void Free();
};

END