#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube final : public CVIBuffer
{
private:
	CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cube(const CVIBuffer_Cube& Prototype);
	virtual ~CVIBuffer_Cube() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3 & minpos, const _float3 & maxpos);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Cube* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3& MinPos, const _float3& MaxPos);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END