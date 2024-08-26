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
	enum OBJECT_CHECK_TYPE {KIT, DEBRIS , SIGN , WALL , RUSSIAN_SIGN , CITY ,BUILDING , MONSTER , ETC, OBJECT_CHECK_TYPE_END};
	// enum ANIM_OBJECT_CHECK_TYPE { PLAYER , ANIM_OBJECT_CHECK_TYPE_END};

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
	static _bool	IsPicking()
	{
		if (m_pPickedObj)
			return true;
		else
			return false;
	}

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT ImGui_GridRender() override;


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
	OBJECT_CHECK_TYPE	m_eObjectCheckType = { OBJECT_CHECK_TYPE::OBJECT_CHECK_TYPE_END };


	_uint				m_iNumModelList[OBJECT_CHECK_TYPE::OBJECT_CHECK_TYPE_END] = { 0 };
	

	_bool				m_bIsCreatePrewViewObject = false;
	PREVIEW_OBJECT		m_tPreViewObject = { nullptr ,nullptr };
	
	static CGameObject* m_pPickedObj;


	class CFreeCamera*  m_pFreeCamera = { nullptr };
	


private:
	HRESULT Ready_Camera();
	HRESULT	Ready_ImGui();
		
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
	HRESULT Ready_Object();



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