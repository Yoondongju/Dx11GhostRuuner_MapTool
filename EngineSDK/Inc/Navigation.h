#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_int			iCurrentIndex = {-1};
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	vector<class CCell*>&	Get_Cells() { return m_Cells; }

	void					Set_OwnerWorldMatrix(const _float4x4* pWorldMatrix) { m_OwnerWorldMatrix = *pWorldMatrix; }
	void					Set_Modifiable(_bool b) { m_bUnModifiable = b; }

public:
	virtual HRESULT Initialize_Prototype(const _wstring & strNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;

public:
	 void Update(_fmatrix OwnerWorldMatrix);
	_bool isMove(_fvector vPosition, _float* pHeight);

	HRESULT SetUp_Neighbors();
	HRESULT	GetUp_BoundingBox();

	_bool	IsInNavigationRange_About_Local(const _float3& TargetLocalPos);


#ifdef _DEBUG
public:
	HRESULT Render(class CGameObject* pOwner = nullptr);
#endif

private:
	vector<class CCell*>				m_Cells;
	_int								m_iCurrentCellIndex = { -1 };
	_float4x4							m_OwnerWorldMatrix = {};

	
	_bool								m_bPreModifiable = false;
	_bool								m_bUnModifiable = false;

	_float4x4							m_LastWorldMatrix = {};

	_float3								m_vMinPos = {};
	_float3								m_vMaxPos = {};


#ifdef _DEBUG
private:
	class CShader*			m_pShader = { nullptr };
	class CVIBuffer_Cube*   m_pVIBuffer = { nullptr };

#endif



public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _wstring & strNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END