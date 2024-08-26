#include "..\Public\Navigation.h"
#include "Cell.h"

#include "Shader.h"
#include "GameInstance.h"

#include "GameObject.h"



CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent{ Prototype }
	, m_Cells{ Prototype.m_Cells }

#ifdef _DEBUG
	, m_pShader{ Prototype.m_pShader }
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _wstring& strNavigationDataFile)
{
	//XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	//
	//_ulong			dwByte = {};
	//
	//HANDLE			hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//if (0 == hFile)
	//	return E_FAIL;
	//
	//_float3			vPoints[3];
	//
	//while (true)
	//{
	//	ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
	//
	//	if (0 == dwByte)
	//		break;
	//
	//	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
	//	if (nullptr == pCell)
	//		return E_FAIL;
	//
	//	m_Cells.emplace_back(pCell);
	//}
	//
	//CloseHandle(hFile);
	//
	//if (FAILED(SetUp_Neighbors()))
	//	return E_FAIL;

#ifdef _DEBUG

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentIndex;

	return S_OK;
}

void CNavigation::Update(_fmatrix OwnerWorldMatrix)
{
	if (false == m_bUnModifiable)
	{
		if (m_bPreModifiable != m_bUnModifiable)		// �� �ѹ��� ������ �� ���ſ� ���� ��¿������
		{
			for (auto& Cell : m_Cells)
			{
				for (_uint i = 0; i < CCell::POINT_END; i++)
				{
					_vector vpPoint = Cell->Get_Point((CCell::POINT)(i));

					vpPoint = XMVector3TransformCoord(vpPoint, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LastWorldMatrix)));
					
					Cell->Set_Point((CCell::POINT)i, vpPoint);
				}
			}
		}
		XMStoreFloat4x4(&m_OwnerWorldMatrix, OwnerWorldMatrix);
		m_LastWorldMatrix = m_OwnerWorldMatrix;
	}
	else
		XMStoreFloat4x4(&m_OwnerWorldMatrix, XMMatrixIdentity());

		
	// �׺��带 Ű������ �����Ӱ� �̵�����
	// �׺��带 ų��� ���������� �̵��ߴ� ������ �˰��־���Ѵ�. �׺����϶� !!����ٲ�� ����!! �ƿ� �������̰� ���ƾ��ҵ�
	// �׺��� ���� ���������� �̵��ߴ� ������ ������� ���ؼ� ���÷� �ٲ��ְ� �״����� �� ������ ���ϸ� �� ����������


	m_bPreModifiable = m_bUnModifiable;
}



_bool CNavigation::isMove(_fvector vPosition , _float* pHeight)
{
	// ���� ���������ǰ� Terrain�� ���忪����� ���� �ͷ����� ���ÿ��� �񱳸� ���ش�.
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_OwnerWorldMatrix)));

	_int		iNeighborIndex = { -1 };

	/* ���� �ִ� �ﰢ�� �ȿ��� �����ΰž�. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex , pHeight))
	{
		_vector vLocalPosition = XMVectorSet(0.0f, *pHeight, 0.0f, 1.0f);
		_vector vWorldPosition = XMVector3TransformCoord(vLocalPosition, XMLoadFloat4x4(&m_OwnerWorldMatrix));
		*pHeight = XMVectorGetY(vWorldPosition);

		
		return true;
	}

	/* ���� �ִ� �ﰢ���� ����ž�. */
	else
	{
		/* �����ʿ� �̿��� �ִٶ��. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex, pHeight))
				{
					_vector vLocalPosition = XMVectorSet(0.0f, *pHeight, 0.0f, 1.0f);
					_vector vWorldPosition = XMVector3TransformCoord(vLocalPosition, XMLoadFloat4x4(&m_OwnerWorldMatrix));
					*pHeight = XMVectorGetY(vWorldPosition);

					break;
				}
					
			}


			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}

		/* �����ʿ� �̿��� ���ٶ��. */
		else
			return false;
	}
}

#ifdef _DEBUG

HRESULT CNavigation::Render(CGameObject* pOwner)
{
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4		vColor = -1 == m_iCurrentCellIndex ? _float4(0.f, 1.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);

	if (nullptr != pOwner && L"Layer_Camera" == pOwner->Get_LayerName())
	{
		//m_OwnerWorldMatrix = 
	}

	_float4x4	WorldMatrix = m_OwnerWorldMatrix; // �긦 ��� ó���ؾ��ұ�? �̹� VIBuffer_Cell�� ���ؽ��� ä�����־� �̰� �ٽø����?

	if (-1 != m_iCurrentCellIndex)
		WorldMatrix._42 += 0.2f;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
	m_pShader->Begin(0);


	if (-1 == m_iCurrentCellIndex)
	{
		for (auto& pCell : m_Cells)
			pCell->Render();
	}
	else
	{
		if(m_Cells.size() > m_iCurrentCellIndex)
			m_Cells[m_iCurrentCellIndex]->Render();
	}


	if (m_pVIBuffer)	// Cube	���ʿ� nullptr �̸� �ٿ���ڽ��� ���������������
	{		
		_vector vCamPos = m_pGameInstance->Find_Camera()->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_matrix InverseWorldMatrix = {};
		InverseWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_OwnerWorldMatrix));

		vCamPos = XMVector3TransformCoord(vCamPos , InverseWorldMatrix);


		_float3 CamPos = {};
		XMStoreFloat3(&CamPos, vCamPos);

		if(IsInNavigationRange_About_Local(CamPos))
			vColor = _float4(1.f, 0.f, 0.f, 1.f);
		else
			vColor = _float4(1.f, 1.f, 0.f, 1.f);


		m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
		m_pShader->Begin(0);


		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();
	}


	return S_OK;
}

#endif

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

HRESULT CNavigation::GetUp_BoundingBox()
{
	if (m_Cells.empty())
		return E_FAIL;

	m_Cells[0]->GetMinMax(&m_vMinPos, &m_vMaxPos);


	for (auto& Cell : m_Cells)
	{
		_float3 cellMin, cellMax;
		Cell->GetMinMax(&cellMin, &cellMax);

		m_vMinPos.x = min(m_vMinPos.x, cellMin.x);
		m_vMinPos.y = min(m_vMinPos.y, cellMin.y);
		m_vMinPos.z = min(m_vMinPos.z, cellMin.z);

		m_vMaxPos.x = max(m_vMaxPos.x, cellMax.x);
		m_vMaxPos.y = max(m_vMaxPos.y, cellMax.y);
		m_vMaxPos.z = max(m_vMaxPos.z, cellMax.z);
	}

	
	if (m_pVIBuffer)
	{
		Safe_Release(m_pVIBuffer);
	}

	m_pVIBuffer = CVIBuffer_Cube::Create(m_pDevice, m_pContext, m_vMinPos, m_vMaxPos);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;


	
	return S_OK;
}

_bool CNavigation::IsInNavigationRange_About_Local(const _float3& TargetLocalPos)
{
	return (TargetLocalPos.x >= m_vMinPos.x && TargetLocalPos.x <= m_vMaxPos.x) &&
		(TargetLocalPos.y >= m_vMinPos.y && TargetLocalPos.y <= m_vMaxPos.y) &&
		(TargetLocalPos.z >= m_vMinPos.z && TargetLocalPos.z <= m_vMaxPos.z);
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
#endif
}
