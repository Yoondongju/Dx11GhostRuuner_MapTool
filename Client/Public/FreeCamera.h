#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CFreeCamera final : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{
		_float		fSensor;
	}CAMERA_FREE_DESC;


public:
	void		 Set_PickedObj(CGameObject* pGameObj);

	CGameObject* Get_PickedObj() { return m_pPickedObj; }

	_float4		 Get_OffsetByPlayer() { return m_OffsetByPlayer; }

	const _float4x4& Get_RotationMatrix() { return m_RotationMatrix; }

private:
	CFreeCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFreeCamera(const CFreeCamera& Prototype);
	virtual ~CFreeCamera() = default;

public:
	CNavigation*	Get_Navigation() override { return m_pCurNavigationCom; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float				m_fSensor = { 0.f };

	CGameObject*		m_pPickedObj = { nullptr };

	_float				m_fAccTime = { 0.f };

	CNavigation*		m_pCurNavigationCom = { nullptr };

private:
	// ¿ÀÇÁ¼Â BY Player 
	_float4					m_OffsetByPlayer = {};
	POINT					m_ptOldMousePos = {};
	_float4x4				m_RotationMatrix = {};


private:
	void	MoveToPickedObj(_float fTimeDelta);

public:
	static CFreeCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END