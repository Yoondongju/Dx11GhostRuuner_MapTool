#pragma once

#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLine.h"

/* 1. 엔진과 클라이언트의 소통을 위한 클래스읻. */
/* 2. 엔진에서 클라이언트에 보여주고 싶은 함수들을 모아놓는다. */
/* 3. 함수들 -> 클래스의 멤버함수. 객체가 필요하다! 그래서 기능응ㄹ 제공해주기위한 클래스 객체를 보관한다. */
/* 4. 객체들을 보관하기위해 Initialize_Engine() 함수 안에서 기능을 제공해주기위한 클래스들을 객체화한다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	POINT Get_MosePos();

public:
	HRESULT Initialize_Engine(_uint iNumLevels , const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);

	void FinalUpdate_Engine();

	HRESULT Draw_Engine();
	HRESULT Clear(_uint iLevelIndex);


#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin();  					  // Get함수 안쓰고 Graphic_Device의 함수 사용하기위해 내부에서
	void Render_End(HWND hWnd = nullptr);	  // Get함수 안쓰고 Graphic_Device의 함수 사용하기위해 내부에서

	void RenderText();

#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Change_Level(_uint iLevelIndx , class CLevel* pNextLevel);
	class CLevel* GetCurLevel();
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	HRESULT Add_Prototype(const _wstring & strPrototypeTag, class CGameObject* pPrototype); // 레벨 인덱스가 없는거 OBJ
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const _wstring & strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex = 0);

	
	//bool IsObjectAtPosition(_uint iLevelIndex, const _wstring & strLayerTag, _float3 vPosition);
	class CGameObject* ObjectAtPosition(_uint iLevelIndex, const _wstring & strLayerTag, _float3 vPosition);


	class CGameObject* Find_Player();
	class CGameObject* Find_Camera();

	class CGameObject* IsObjectAtPosition(_uint iLevelIndex, const _wstring & strLayerTag, _float3 vPosition);
	CGameObject* IsObjectAtPosition_GoodVerdict(_uint iLevelIndex, const _wstring & strLayerTag, _float3 vPosition);

	list<class CGameObject*>& Get_GameObjects(_uint iLevelIndx, const _wstring & strLayerTag);
#pragma endregion


#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag, class CComponent* pPrototype); // 레벨 인덱스가 있는거 컴포넌트
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, void* pArg = nullptr);
	class CComponent* Get_Prototype(_uint iNumLevelIndex, const _wstring & strPrototypeTag);	
#pragma endregion


#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring & strTimerTag);
	_float Compute_TimeDelta(const _wstring & strTimerTag);
	void Update_Timer(const _wstring & strTimerTag);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	list<class CGameObject*>& Get_RenderList(CRenderer::RENDERGROUP eGroup);

#pragma endregion


#pragma region KEY_MANAGER
	KEY_STATE	Get_KeyState(KEY _eKey);
#pragma endregion

#pragma region EVENT_MANAGER
	HRESULT Event_CreateObject(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, _uint iSize = 0, void* pArg = nullptr);
	HRESULT Event_DestoryObject(class CGameObject* pGameObject);
#pragma endregion


#pragma region EVENT_MANAGER
	//HRESULT Event_CreateObject(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, _uint iSize = 0, void* pArg = nullptr);
	//HRESULT Event_DestoryObject(class CGameObject* pGameObject);
#pragma endregion



#pragma region PIPELINE
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;

	_float4 Get_CamPosition_Float4() const;
	_vector Get_CamPosition_Vector() const;
#pragma endregion




#pragma region PICKING_MANAGER
	_float3 Get_PickingObject_CenterPos(_uint iLevelIndex , const _wstring & strLayerTag);
	_float3 Get_PickingObject_JustPos(_uint iLevelIndex, const _wstring& strLayerTag);
	_float3 Get_PickingBackGround_JustPos(_float fTargetY);
		

	void Transform_MouseRay_ToLocalSpace(const _float4x4 & WorldMatrix);

	_bool isPicked_InLocalSpace(const BoundingBox& MeshBoundingBox, _float3* pOut);
	_bool isPicked_InLocalSpace(const _vector& vPointA, const _vector& vPointB, const _vector& vPointC, _vector* pOut);



	_bool isPicked_InWorldSpace(const BoundingBox& MeshBoundingBox, _float3* pOut);

#pragma endregion

	// 이벤트 매니저를 통한 삭제 로직
	void Delete(_uint iLevelIndex, CRenderer::RENDERGROUP eRenderGroup , class CGameObject* pObj);


#pragma region PHYSX_MANAGER
	PxPhysics* Get_Physics();	
	PxScene* Get_Scene();		
	PxMaterial* Get_Material(); 
#pragma endregion


private:
	HWND						m_hWnd = {};

	class CGraphic_Device*		m_pDevice = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CComponent_Manager*	m_pComponent_Manager = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };



	/* 여기서부터 우리가 추가한 기능들*/
	class CKey_Manager*		m_pKey_Manager = { nullptr };
	class CEvent_Manager*	m_pEvent_Manager = { nullptr };
	class CUI_Manager*		m_pUI_Manager = { nullptr };
	class CPickingManager*  m_pPicking_Manager = { nullptr };

	class CPhysXManager*	m_pPhysX_Manager = { nullptr };


public:
	void Release_Engine();
	virtual void Free() override;

};

END

