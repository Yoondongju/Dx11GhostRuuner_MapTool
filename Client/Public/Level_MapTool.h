#pragma once

#include "Client_Defines.h"


#include "Level.h"


BEGIN(Engine)
class CTexture;
class CGameObject;
END



BEGIN(Client)

class CLevel_MapTool final : public CLevel
{
public:
	enum MAIN_CHECK_TYPE { TERRAIN, OBJECT, CAMERA , NAVIGATION,MAIN_CHECK_TYPE_END};
	enum OBJECT_TYPE
	{
		DECORATIVE_OBJECT,
		STATIC_OBJECT,
		DYNAMIC_OBJECT,


		OBJECT_TYPE_END,
	};
	enum MODEL_CHECK_LIST {
		KIT,
		DEBRIS ,
		BARRICADE,
		BILLBOARD,
		BOSS_FIGHT1,
		BOSS_FIGHT3,
		CATWALK,
		CRANE,
		CYBERCITY,
		DECO,
		FAN,
		INDUSTRIAL_ELEVATOR,
		LAMP,
		PIPE,
		PLATFORM,
		ROTATIONFAN,
		TRAIN,
		SIGN , 
		WALL , 
		RUSSIAN_SIGN ,
		CITY ,
		BUILDING ,
		WIRE,
		BODYBAG,
		CLIMBOBJECT,

		SPIDER,		
		ELITE,
		JETPACK,
		MIRA,
		PISTOL,
		SNIPER,

		ETC, 
		MODEL_CHECK_TYPE_END
	};


public:
	typedef struct
	{
		CGameObject*		pTargetObject = { nullptr };
		void*			    pAdditionInfo = { nullptr };
	}PREVIEW_OBJECT;


private:
	CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MapTool() = default;


public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT ImGui_GridRender() override;

public:
	static _bool	IsPicking()
	{
		if (m_pPickedObj)
			return true;
		else
			return false;
	}


private:
	_char		m_szVerticesX[50]= "";
	_char		m_szVerticesZ[50]= "";

	_float3		m_CellPoints[3] = {};

	_uint		m_iPointIndex = {0};
	_int		m_iCellIndex = { 0 };

	_float3		m_MousePos = {};
	_float		m_fMouseY = {};


	_bool				m_bPerspectiveMode = true;
	MAIN_CHECK_TYPE		m_eMainCheckType = { MAIN_CHECK_TYPE::MAIN_CHECK_TYPE_END };


	MODEL_CHECK_LIST	m_eModelCheckType = { MODEL_CHECK_LIST::MODEL_CHECK_TYPE_END };




	

	_bool				m_bIsCreatePrewViewObject = false;
	PREVIEW_OBJECT		m_tPreViewObject = { nullptr ,nullptr };
	
	static CGameObject* m_pPickedObj;


	class CFreeCamera*  m_pFreeCamera = { nullptr };
	


private:
	HRESULT Ready_Camera();
	HRESULT	Ready_ImGui();

	HRESULT Ready_Object();

	void    ImGui_Render();					// 기본 ImGui
	void    Second_ImGui_Render();			// 현재 레벨에 있는 오브젝트의 갯수표현
	void	Third_ImGui_Render();			// 애니메이션에 관련된 정보들을 표현


private:
	HRESULT Create_PreViewObject(_int iSelectObjectNum);
	HRESULT Create_Object();



private:
	_bool	IsPicking_ByObjects(CGameObject** pOut);



private:
	void Open_MapDialog();				// 맵

	void Open_ObjectDialog();			// 오브젝트

	void Open_CameraDialog();			// 카메라 

	void Open_NavigationDialog();		// 네비게이션

	void Open_UIDialog();				// UI






private:
	void Save_Map();

	void Save_Terrain();
	void Save_NonAnimObject();
	void Save_AnimObject();

	void Save_Navigation();

private:
	void Save_NonAnimObject_MapTool();
	void Save_AnimObject_MapTool();

private:
	void Save_Player();



private:
	void Load_Map();

	void Load_Terrain();
	void Load_NonAnimObject_MapTool();
	void Load_AnimObject_MapTool();
	
	void Load_Navigation();


	void ReadString(ifstream& fin, string& str);





public:
	static CLevel_MapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END