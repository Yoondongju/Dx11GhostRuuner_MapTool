
#include "..\Public\VIBuffer_GridRect.h"

#include "../Public/GameInstance.h"


CVIBuffer_GridRect::CVIBuffer_GridRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice,pContext }
{
	m_eComponentType = VIBUFFER_RECT;
}

CVIBuffer_GridRect::CVIBuffer_GridRect(const CVIBuffer_GridRect& Prototype)
	: CVIBuffer{ Prototype }
{
	m_eComponentType = VIBUFFER_RECT;
}

HRESULT CVIBuffer_GridRect::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* �������ۿ� ä���� ������ ��������ؼ� �ӽ������� ������ �Ҵ��Ѵ�. */
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.f, 0.5f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.f, 0.5f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, 0.f, -0.5f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, 0.f, -0.5f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	// ó���� VTXPOSTEX �� �������ְ� BufferDesc�� �������ְ� �״����� InitailData�� ��������� ��Ȱ���

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* �������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* �ε��������� ������ ä���ְ� */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* �ε������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}
HRESULT CVIBuffer_GridRect::Initialize(void* pArg)
{
	return S_OK;
}

_bool CVIBuffer_GridRect::isPicking(const _fmatrix& WorldMatrix, _float3* pOut)
{
//	m_pGameInstance->Transform_MouseRay_ToLocalSpace(WorldMatrix);
//
//
//	/* ������ �� �ﰢ�� */
//	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[0],
//		m_pVerticesPos[1],
//		m_pVerticesPos[2],
//		pOut))
//		goto Compute_WorldPos;
//
//
//	/* ���� �Ʒ� �ﰢ�� */
//	if (true == m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[0],
//		m_pVerticesPos[2],
//		m_pVerticesPos[3],
//		pOut))
//		goto Compute_WorldPos;
//
//	return false;
//
//Compute_WorldPos:
//	D3DXVec3TransformCoord(pOut, pOut, &WorldMatrix);
//	return true;

	return S_OK;
}

_bool CVIBuffer_GridRect::IsPick(CTransform* pTransform, _float3* pOut)
{
	//if (m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	//{
	//	m_pGameInstance->Change_To_LocalSpace(pTransform->Get_WorldMatrix_Inverse());
	//
	//	if (m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[0], m_pVerticesPos[1], m_pVerticesPos[2], pOut))
	//		return true;
	//
	//	if (m_pGameInstance->isPicked_InLocalSpace(m_pVerticesPos[0], m_pVerticesPos[2], m_pVerticesPos[3], pOut))
	//		return true;
	//}

	return false;
}



CVIBuffer_GridRect* CVIBuffer_GridRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_GridRect* pInstance = new CVIBuffer_GridRect(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_GridRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* ������ü�� ã�Ƽ� ������ü�� Clone�Լ��� ȣ���Ѵ�.  */
CComponent* CVIBuffer_GridRect::Clone(void* pArg)
{
	CVIBuffer_GridRect* pInstance = new CVIBuffer_GridRect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_GridRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_GridRect::Free()
{
	__super::Free();

}

