#pragma once
#include "Base.h"




BEGIN (Engine)

class CEventMgr final : public CBase
{
public:
	enum EVENT_TYPE { DELETE_OBJECT, CREATE_OBJECT, CHANGE_LEVEL,CHANGE_STATE };
	
	typedef struct tCloneObj_Desc
	{
		_uint iLevelIndex;
		_wstring strLayerTag;
		_wstring strPrototypeTag;
		void* pArg = nullptr;

	}CLONE_DESC;

	typedef struct tEvent
	{
		EVENT_TYPE				eEven;
		DWORD_PTR				lParam;
		DWORD_PTR				wParam;
		
		CLONE_DESC				CloneDesc{};
	}EVENT;


	


private:
	CEventMgr(class CLevel_Manager* pLevelMgr, class CObject_Manager* pObjectMgr);
	virtual ~CEventMgr() = default;

public:
	HRESULT Initialize();
	void update();

public:
	void AddEvent(const tEvent& _eve) { m_Eventlist.push_back(_eve); }

private:
	class CLevel_Manager*     m_pLevel_Manager;
	class CObject_Manager*	  m_pObject_Manager;
	

	list<tEvent>			  m_Eventlist;
	list<class CGameObject*>  m_Deadlist;




private:
	void Excute(const tEvent& _eve);


public:
	static CEventMgr* Create(class CLevel_Manager* pLevelMgr, class CObject_Manager* pObjectMgr);
	virtual void Free() override;
};

END
