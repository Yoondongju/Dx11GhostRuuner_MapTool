#include "..\Public\VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice,pContext }
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Cube::Initialize_Prototype(const _float3& minpos, const _float3& maxpos)
{
	m_iVertexStride = sizeof(VTXCUBE);
	m_iNumVertices = 8;
	m_iNumVertexBuffers = 1;
	m_iNumFaces = 12;
	m_iIndexStride = 2;
	m_iNumIndices = 36;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;


	VTXCUBE* pVertices = new VTXCUBE[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCUBE) * m_iNumVertices);

	pVertices[0].vPosition = _float3(minpos.x, maxpos.y, minpos.z);
	pVertices[0].vTexcoord = pVertices[0].vPosition;

	pVertices[1].vPosition = _float3(maxpos.x, maxpos.y, minpos.z);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	pVertices[2].vPosition = _float3(maxpos.x, minpos.y, minpos.z);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	pVertices[3].vPosition = _float3(minpos.x, minpos.y, minpos.z);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	pVertices[4].vPosition = _float3(minpos.x, maxpos.y, maxpos.z);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	pVertices[5].vPosition = _float3(maxpos.x, maxpos.y, maxpos.z);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	pVertices[6].vPosition = _float3(maxpos.x, minpos.y, maxpos.z);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	pVertices[7].vPosition = _float3(minpos.x, minpos.y, maxpos.z);
	pVertices[7].vTexcoord = pVertices[7].vPosition;
	
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);


	// 큐브의 모서리를 그리기 위한 인덱스 설정
	pIndices[0] = 0; pIndices[1] = 1;  // 상단 앞쪽 선
	pIndices[2] = 1; pIndices[3] = 2;  // 앞쪽 오른쪽 선
	pIndices[4] = 2; pIndices[5] = 3;  // 하단 앞쪽 선
	pIndices[6] = 3; pIndices[7] = 0;  // 앞쪽 왼쪽 선

	pIndices[8] = 4; pIndices[9] = 5;  // 상단 뒤쪽 선
	pIndices[10] = 5; pIndices[11] = 6;  // 뒤쪽 오른쪽 선
	pIndices[12] = 6; pIndices[13] = 7;  // 하단 뒤쪽 선
	pIndices[14] = 7; pIndices[15] = 4;  // 뒤쪽 왼쪽 선

	pIndices[16] = 0; pIndices[17] = 4;  // 상단 왼쪽 선
	pIndices[18] = 1; pIndices[19] = 5;  // 상단 오른쪽 선
	pIndices[20] = 2; pIndices[21] = 6;  // 하단 오른쪽 선
	pIndices[22] = 3; pIndices[23] = 7;  // 하단 왼쪽 선


	///* +x */
	//pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	//pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;
	//
	///* -x */
	//pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	//pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;
	//
	///* +y */
	//pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	//pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;
	//
	///* -y */
	//pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	//pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;
	//
	///* +z */
	//pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	//pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;
	//
	///* -z */
	//pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	//pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , const _float3& MinPos, const _float3& MaxPos)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(MinPos,MaxPos)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();

}

