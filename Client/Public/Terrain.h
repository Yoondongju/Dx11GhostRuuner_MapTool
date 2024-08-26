#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
class CNavigation;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	_uint	Get_TextureIndex() { return m_iTextureIndex; }

	_uint	Get_iNumX() { return m_iNumX; }
	_uint	Get_iNumZ() { return m_iNumZ; }

	CNavigation* Get_Navigation() override { return m_pNavigationCom; }

public:
	virtual HRESULT Initialize_Prototype(_uint iNumX, _uint iNumZ);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	HRESULT	Change_VIBuffer_Terrain(const _wstring& VIBuffer_Terrain_PrototypeName);


private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom[2] = {nullptr};

	CNavigation*		m_pNavigationCom = { nullptr };
	CVIBuffer_Terrain*  m_pVIBufferCom = { nullptr };

	_uint				m_iTextureIndex = {};


	_uint				m_iNumX;
	_uint				m_iNumZ;

private:
	HRESULT Ready_Components();



public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumX ,_uint iNumZ);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END