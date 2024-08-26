#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CLandObject abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		CVIBuffer_Terrain*					pTerrainVIBuffer = { nullptr };
		CTransform*							pTerrainTransform = { nullptr };
		
	}LANDOBJECT_DESC;

public:
	static void Set_TerrainVIBuffer(CVIBuffer_Terrain* pTerrainBuffer) 
	{ 
		m_pTerrainVIBuffer = pTerrainBuffer;  
		//Safe_AddRef(m_pTerrainVIBuffer);		// 객체를 만들지 않을것이라 레퍼런스 카운트 증가 안시킬거임
	}

	static void Set_TerrainTransform(CTransform* pTerrainTransform)
	{
		m_pTerrainTransform = pTerrainTransform;
		//Safe_AddRef(m_pTerrainTransform);
	}

protected:
	CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLandObject(const CLandObject& Prototype);
	virtual ~CLandObject() = default;
	

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



public:
	static HRESULT SetUp_OnTerrain(CTransform* pTransform, _float fOffsetY = 0.f);


protected:
	static CVIBuffer_Terrain*  m_pTerrainVIBuffer;
	static CTransform*		   m_pTerrainTransform;





public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END