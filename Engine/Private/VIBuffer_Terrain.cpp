
#include "..\Public\VIBuffer_Terrain.h"

#include "../Public/GameInstance.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice,pContext }
{

}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumVerticesX { Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ { Prototype.m_iNumVerticesZ }
	, m_pIndices { Prototype.m_pIndices}
{

}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ ,const _tchar* pHeightMapFilePath)
{
	m_iNumVerticesX = iNumVerticesX;   // 가로 세로를 기억해야해 
	m_iNumVerticesZ = iNumVerticesZ;   // 가로 세로를 기억해야해 


	m_iNumVertexBuffers = 1;						// 내 버텍스버퍼가 몇개니?
	m_iNumVertices = iNumVerticesX * iNumVerticesZ;	// 내 버텍스 갯수가 몇개니? 가로 X * 세로 Z	
	m_iVertexStride = sizeof(VTXNORTEX);   // 조명연산좀 해보자 // 내 버텍스한개의 크기는 몇이니 ?
	m_pVerticesPos = new _float3[m_iNumVertices];	// 내가 추후 저장할버텍스 좌표점
	
	
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;



#pragma region VERTEX_BUFFER
	// 정점 버퍼에 채워줄 값을 만들기위해 임시로 공간을 할당한다.	
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX));
	
	for (size_t i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (size_t j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = (i * m_iNumVerticesX) + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j, 0.f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f) , i / (m_iNumVerticesZ - 1.f));
		}
	}
#pragma endregion

	

#pragma region INDEX_BUFFER

	_uint*		pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint iNumIndices = {0};

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX, /* 왼쪽 위 정점의 인덱스 */
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

		
			vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);


			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}

	}

	for (size_t i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
#pragma endregion
	
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;


	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;


	memcpy(m_pIndices, pIndices, sizeof(_uint)* m_iNumIndices);

	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pVertices);


	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}



_bool CVIBuffer_Terrain::isPick(class CTransform* pTransform, _float3* pOut)
{
	_float4x4 Matrix = {};
	XMStoreFloat4x4(&Matrix, pTransform->Get_WorldMatrix());

	m_pGameInstance->Transform_MouseRay_ToLocalSpace(Matrix);

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1 ,
				iIndex,
			};


			if (m_pGameInstance->isPicked_InLocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
				XMLoadFloat3(&m_pVerticesPos[iIndices[1]]),
				XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
				&XMLoadFloat3(pOut)))
				return true;

			else if (m_pGameInstance->isPicked_InLocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
				XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
				XMLoadFloat3(&m_pVerticesPos[iIndices[3]]),
				&XMLoadFloat3(pOut)))
				return true;

		}
	}

	return false;
}

_float CVIBuffer_Terrain::Compute_Height(_fvector vLocalPos)
{
	///* 객체가 존재하는 네모 영역의 왼쪽 하단 인덱스를 구했다. */
	_uint		iIndex = _uint(XMVectorGetZ(vLocalPos)) * m_iNumVerticesX + _uint(XMVectorGetX(vLocalPos));
	

	if (XMVectorGetX(vLocalPos) < 0 || XMVectorGetX(vLocalPos) >= m_iNumVerticesX - 1 ||
		XMVectorGetZ(vLocalPos) < 0 || XMVectorGetZ(vLocalPos) >= m_iNumVerticesZ - 1)
	{
		// 유효 범위를 벗어났다면
		return 0.f;
	}

	
	/* 왼쪽하단이넫ㄱ스 기준으로 네방향의 ㄱ꼭지점 인덷ㄱ스를 구하자.  */
	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX, /* 왼위*/
		iIndex + m_iNumVerticesX + 1, /* 오위 */
		iIndex + 1, /* 오아 */
		iIndex /* 왼아 */
	};
      
	

	_float		fWidth = XMVectorGetX(vLocalPos) - m_pVerticesPos[iIndices[0]].x;
	_float		fDepth = m_pVerticesPos[iIndices[0]].z - XMVectorGetZ(vLocalPos);
	
	_vector		Plane{};
	
	


	/* 오른쪽 위 삼각형안에있어. */
	if (fWidth > fDepth)
	{
		Plane = XMPlaneFromPoints(
			XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[1]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]])
		);	
	}
	/* 왼쪽 아래 삼각형안에있어. */
	else
	{
		Plane = XMPlaneFromPoints(
			XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[3]])
		);	
	}
	
	XMFLOAT4 planeCoefficients;
	XMStoreFloat4(&planeCoefficients, Plane);

	return (-planeCoefficients.x * XMVectorGetX(vLocalPos) - planeCoefficients.z * XMVectorGetZ(vLocalPos) - planeCoefficients.w) / planeCoefficients.y;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesZ, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumVerticesX, iNumVerticesZ, pHeightMapFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
	

	if (false == m_isCloned)		// 원본이면 
	{
		Safe_Delete_Array(m_pIndices);
	}
		
}

