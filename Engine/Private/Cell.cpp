#include "..\Public\Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_A])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;
	}

	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_B])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
	}

	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_C])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex, _float* pHeight)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSour, vDest;

		vSour = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vPoints[i]);
		vDest = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

		if (0 < XMVectorGetX(XMVector3Dot(vSour, vDest)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];	// 셀 안에 없네?
			return false;
		}
	}



	_vector vEdge1 = XMLoadFloat3(&m_vPoints[1]) - XMLoadFloat3(&m_vPoints[0]);
	_vector vEdge2 = XMLoadFloat3(&m_vPoints[2]) - XMLoadFloat3(&m_vPoints[0]);

	_vector vNormalVector = XMVector3Cross(vEdge1, vEdge2);

	// 법선 벡터의 성분을 가져옵니다
	_float A = XMVectorGetX(vNormalVector);
	_float B = XMVectorGetY(vNormalVector);
	_float C = XMVectorGetZ(vNormalVector);

	// 평면의 D값을 계산합니다
	_float D = -XMVectorGetX(XMVector3Dot(vNormalVector, XMLoadFloat3(&m_vPoints[0])));
	// 법선벡터의 성분과 point0 x,y,z의 성분을 내적한다 각 성분끼리 곱한다

	if (pHeight != nullptr)
	{
		_vector vPos = vPosition;
		_float fX = XMVectorGetX(vPos);
		_float fZ = XMVectorGetZ(vPos);
		*pHeight = (-D - A * fX - C * fZ) / B;
	}

	return true;
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
