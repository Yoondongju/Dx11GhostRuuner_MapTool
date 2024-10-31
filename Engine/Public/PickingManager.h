#pragma once
#include "Base.h"


BEGIN (Engine)
class CPickingManager final : public CBase
{
private:
	CPickingManager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CPickingManager() = default;

public:
	HRESULT	Initialize(HWND hWnd);
	_bool Picking(_float3* pPickPos);
	_vector Compute_Height(_fvector vWorldPos, _fmatrix ViewMatrix, _fmatrix ProjMatrix);



public:
	_float3 Get_PickingObject_CenterPos(class CLayer* pLayer);
	_float3 Get_PickingObject_JustPos(class CLayer* pLayer);

	_float3 Get_PickingBackGround_JustPos(_float fTargetY);




public:
	void Update(); /* 윈도우좌표상(뷰포트상)의 마우스 좌표를 구하고 이 마우스를 좌표를 공용 스페이스(월드 스페이스)까지 내릴 수 있을 만큼 내린다. */
	void Transform_ToLocalSpace(const _float4x4& WorldMatrix);

	_bool isPicked_InWorldSpace(const BoundingBox& MeshBoundingBox, _float3* pOut);


	_bool isPicked_InLocalSpace(const BoundingBox& MeshBoundingBox, _float3* pOut);
	_bool isPicked_InLocalSpace(const _vector& vPointA, const _vector& vPointB, const _vector& vPointC, _vector* pOut);


private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	ID3D11Texture2D*				m_pPickDepthTexture = { nullptr };
	_uint							m_iViewportWidth{}, m_iViewportHeight{};


	class CGameInstance*			m_pGameInstance = { nullptr };
	_float3							m_vOriginal;
	_float3							m_vDirection;
	HWND							m_hWnd = {};
	

private:
	_float3						m_vRayPos_InLocalSpace = {};
	_float3						m_vRayDir_InLocalSpace = {};

public:
	static CPickingManager* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext , HWND hWnd);
	virtual void Free() override;
};
END

