#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{

private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;


public:
	virtual HRESULT Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ, const _tchar * pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;


public:
	virtual _bool isPick(class CTransform * pTransform, _float3 * pOut);

public:
	_float Compute_Height(_fvector vLocalPos);



private:
	_uint			m_iNumVerticesX = {};
	_uint			m_iNumVerticesZ = {};

	_uint*			m_pIndices = nullptr;


public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext ,_uint iNumVerticesX, _uint iNumVerticesZ, const _tchar * pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
