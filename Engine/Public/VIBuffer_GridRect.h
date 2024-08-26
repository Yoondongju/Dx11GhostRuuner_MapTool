#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_GridRect final : public CVIBuffer
{
private:
	CVIBuffer_GridRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_GridRect(const CVIBuffer_GridRect& Prototype);
	virtual ~CVIBuffer_GridRect() = default;

public:
	_float3* Get_pVerticesPos() { return m_pVerticesPos; }		// 무조건 4개 겟지


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;


public:
	virtual _bool isPicking(const _fmatrix& WorldMatrix, _float3* pOut) override;


public:
	virtual _bool IsPick(class CTransform* pTransform, _float3 * pOut = nullptr);



public:
	static CVIBuffer_GridRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END