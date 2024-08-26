#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	void	Set_Point(POINT ePoint, _fvector Point) { XMStoreFloat3(&m_vPoints[ePoint] , Point); }



	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

	void GetMinMax(_float3* pMinPos, _float3* pMaxPos) const
	{
		pMinPos->x = min(m_vPoints[POINT_A].x, min(m_vPoints[POINT_B].x, m_vPoints[POINT_C].x));
		pMinPos->y = min(m_vPoints[POINT_A].y, min(m_vPoints[POINT_B].y, m_vPoints[POINT_C].y));
		pMinPos->z = min(m_vPoints[POINT_A].z, min(m_vPoints[POINT_B].z, m_vPoints[POINT_C].z));


		pMaxPos->x = max(m_vPoints[POINT_A].x, max(m_vPoints[POINT_B].x, m_vPoints[POINT_C].x));
		pMaxPos->y = max(m_vPoints[POINT_A].y, max(m_vPoints[POINT_B].y, m_vPoints[POINT_C].y));
		pMaxPos->z = max(m_vPoints[POINT_A].z, max(m_vPoints[POINT_B].z, m_vPoints[POINT_C].z));
	}



public:
	_int			Get_Index() { return m_iIndex; }

	const _float3*  Get_vPoints() { return m_vPoints; }

	_int*			Get_NeighborIndices() { return m_iNeighborIndices; }
	void			Set_NeighborIndex(LINE eLine, _int iNeighborIndex) { m_iNeighborIndices[eLine] = iNeighborIndex; }


public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool Compare_Points(_fvector vSour, _fvector vDest);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex, _float* pHeight);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	_int							m_iIndex = {};
	_float3							m_vPoints[POINT_END] = {};
	_int							m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

END