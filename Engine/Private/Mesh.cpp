#include "..\Public\Mesh.h"

#include "../Public/GameInstance.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(const CModel* pModel , CModel::MODEL_TYPE eType  , const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;    // 하나의 메쉬는 오직 하나의 머테리얼만 가진다! 그래서 인덱스가 한개임  >> 나의 메쉬 하나는 어떤 머테리얼을 가르키고 그 머테리얼은 구조체로서 그 구조체 안에서 Diffuse, normal , specaulr
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iVertexStride = sizeof(VTXMESH);
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT hr = eType == CModel::TYPE_NONANIM ? Ready_VertexBuffer_NonAnim(pAIMesh, PreTransformMatrix) : Ready_VertexBuffer_Anim(pModel , pAIMesh);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		aiFace		AIFace = pAIMesh->mFaces[i];

		m_pIndices[iNumIndices++] = AIFace.mIndices[0];
		m_pIndices[iNumIndices++] = AIFace.mIndices[1];
		m_pIndices[iNumIndices++] = AIFace.mIndices[2];
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion




	if (m_pVIBuffer)
	{
		Safe_Release(m_pVIBuffer);
	}

	m_pVIBuffer = CVIBuffer_Cube::Create(m_pDevice, m_pContext, m_vMinPos, m_vMaxPos);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;




	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(const CModel* pModel, CShader* pShader, const _char* pConstantName)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iMaxMeshBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * pModel->Get_BoneCombindTransformationMatrix(m_BoneIndices[i]));
	}

	return  pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

_bool CMesh::isPicking(const _fmatrix& WorldMatrix, _float3* pOut)
{
	// 인자로 월드행렬을 받는이유는 만약 로컬스페이스에서 피킹비교할거면 그 객체의 월드행렬의 역행렬이 필요하기 때문임
	// 근데 월드스페이스에서 비교할거면 월드행렬 필요없음

	// 바운딩 박스만들기전에 피킹 불가능 매우 많은 정점을 비교해야하는데 이건 말이안됌

	BoundingBox boundingBox;
	boundingBox.Center = XMFLOAT3((m_vMaxPos.x + m_vMinPos.x) / 2, (m_vMaxPos.y + m_vMinPos.y) / 2, (m_vMaxPos.z + m_vMinPos.z) / 2);
	boundingBox.Extents = XMFLOAT3((m_vMaxPos.x - m_vMinPos.x) / 2, (m_vMaxPos.y - m_vMinPos.y) / 2, (m_vMaxPos.z - m_vMinPos.z) / 2);
	// 이 바운딩 박스는 로컬상에 잇는거다 그래서 추후에 월드행렬 바꿀시 
	// 얘도 자동으로 스케일이 바뀌나봄
	

	_float4x4 vBuffer;
	XMStoreFloat4x4(&vBuffer, WorldMatrix);

	m_pGameInstance->Transform_MouseRay_ToLocalSpace(vBuffer);
	
	// 충돌 검사
	_float3 vPickPos;

	return 	m_pGameInstance->isPicked_InLocalSpace(boundingBox, &vPickPos);

}


HRESULT CMesh::Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&m_pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&m_pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;


	XMStoreFloat3(&m_vMinPos, XMLoadFloat3(&m_pVertices[0].vPosition));
	XMStoreFloat3(&m_vMaxPos, XMLoadFloat3(&m_vMinPos));

	for (size_t i = 0; i < m_iNumVertices; i++)
	{	
	 	XMStoreFloat3(&m_vMinPos , XMVectorMin(XMLoadFloat3(&m_vMinPos), XMLoadFloat3(&m_pVertices[i].vPosition)));
	 	XMStoreFloat3(&m_vMaxPos , XMVectorMax(XMLoadFloat3(&m_vMaxPos), XMLoadFloat3(&m_pVertices[i].vPosition)));
	}



	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(const CModel* pModel, const aiMesh* pAIMesh)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_pAnimVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_pAnimVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	/* 모델 전체의 뼈가 아닌 이 메시하나에 영향을 주는 뼈의 갯수. */
	m_iNumBones = pAIMesh->mNumBones;	

	m_OffsetMatrices.resize(m_iNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];
		
		/* pAIBone->mOffsetMatrix : 여러개의 모델이 하나의 뼈대를 공유하는 경우, 모델의 형태에 따라서 스키닝(정점의 위치 * 뼈행렬)작업 시, 추가적인 보정이 필요할 수 있다.  */
		memcpy(&m_OffsetMatrices[i], &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&m_OffsetMatrices[i], XMMatrixTranspose(XMLoadFloat4x4(&m_OffsetMatrices[i])));

		/* 내 모델이 들고 있는 전체 뼈들중에 이 메시가 사용하는 뼈와이름이 같은 뼈의 인덱스를 저장해둔다. */
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(pAIBone->mName.data));


		m_iNumBoneWeight = pAIBone->mNumWeights;

			// pAIBone 란 이 메쉬에 영향을 주는 뼈들중에 하나 !!
			/* 이 뼈는 몇개(mNumWeights) 정점에게 영향을 주는가? */
			for (size_t j = 0; j < pAIBone->mNumWeights; j++)
			{
				/* 이 뼈가 영향을 주는 j번째 정점의 인덱스(pAIBone->mWeights[j].mVertexId)*/
				// 아직 이 정점에 대해 특정 뼈의 가중치가 할당되지 않았다는 것을 확인하기 위해서입니다.
				// 이 정점이 영향을 받을수있는 뼈 4개중 첫번째가 0이라면 영향을 받을수잇는공간이 잇다는거니까 ! 
				// 받을수 있는 공간에 내 메쉬에 영향을 주는 뼈들중 하나인 pAIBone ! 그녀석은  영향을 주는 뼈들중에서 몇번째? i 번째 !
				if (0.f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
				{
					m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
					m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
				}

				else if (0.f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
				{
					m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
					m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
				}

				else if (0.f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
				{
					m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
					m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
				}

				else
				{
					m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
					m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
				}
			}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(m_szName));

		_float4x4	OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pAnimVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;


	XMStoreFloat3(&m_vMinPos, XMLoadFloat3(&m_pAnimVertices[0].vPosition));
	XMStoreFloat3(&m_vMaxPos, XMLoadFloat3(&m_vMinPos));

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&m_vMinPos, XMVectorMin(XMLoadFloat3(&m_vMinPos), XMLoadFloat3(&m_pAnimVertices[i].vPosition)));
		XMStoreFloat3(&m_vMaxPos, XMVectorMax(XMLoadFloat3(&m_vMaxPos), XMLoadFloat3(&m_pAnimVertices[i].vPosition)));
	}



	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CModel* pModel , CModel::MODEL_TYPE eType  , const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModel , eType , pAIMesh, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	
	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pAnimVertices);

	Safe_Delete_Array(m_pIndices);

	Safe_Release(m_pVIBuffer);
}
