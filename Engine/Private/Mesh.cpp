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
	m_iMaterialIndex = pAIMesh->mMaterialIndex;    // �ϳ��� �޽��� ���� �ϳ��� ���׸��� ������! �׷��� �ε����� �Ѱ���  >> ���� �޽� �ϳ��� � ���׸����� ����Ű�� �� ���׸����� ����ü�μ� �� ����ü �ȿ��� Diffuse, normal , specaulr
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
	/* �ε��������� ������ ä���ְ� */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
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

	/* �ε������۸� �����Ѵ�. */
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
	// ���ڷ� ��������� �޴������� ���� ���ý����̽����� ��ŷ���ҰŸ� �� ��ü�� ��������� ������� �ʿ��ϱ� ������
	// �ٵ� ���彺���̽����� ���ҰŸ� ������� �ʿ����

	// �ٿ�� �ڽ���������� ��ŷ �Ұ��� �ſ� ���� ������ ���ؾ��ϴµ� �̰� ���̾ȉ�

	BoundingBox boundingBox;
	boundingBox.Center = XMFLOAT3((m_vMaxPos.x + m_vMinPos.x) / 2, (m_vMaxPos.y + m_vMinPos.y) / 2, (m_vMaxPos.z + m_vMinPos.z) / 2);
	boundingBox.Extents = XMFLOAT3((m_vMaxPos.x - m_vMinPos.x) / 2, (m_vMaxPos.y - m_vMinPos.y) / 2, (m_vMaxPos.z - m_vMinPos.z) / 2);
	// �� �ٿ�� �ڽ��� ���û� �մ°Ŵ� �׷��� ���Ŀ� ������� �ٲܽ� 
	// �굵 �ڵ����� �������� �ٲ��
	

	_float4x4 vBuffer;
	XMStoreFloat4x4(&vBuffer, WorldMatrix);

	m_pGameInstance->Transform_MouseRay_ToLocalSpace(vBuffer);
	
	// �浹 �˻�
	_float3 vPickPos;

	return 	m_pGameInstance->isPicked_InLocalSpace(boundingBox, &vPickPos);

}


HRESULT CMesh::Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* �������ۿ� ä���� ������ ��������ؼ� �ӽ������� ������ �Ҵ��Ѵ�. */
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

	/* �������۸� �����Ѵ�. */
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
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* �������ۿ� ä���� ������ ��������ؼ� �ӽ������� ������ �Ҵ��Ѵ�. */
	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_pAnimVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_pAnimVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	/* �� ��ü�� ���� �ƴ� �� �޽��ϳ��� ������ �ִ� ���� ����. */
	m_iNumBones = pAIMesh->mNumBones;	

	m_OffsetMatrices.resize(m_iNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];
		
		/* pAIBone->mOffsetMatrix : �������� ���� �ϳ��� ���븦 �����ϴ� ���, ���� ���¿� ���� ��Ű��(������ ��ġ * �����)�۾� ��, �߰����� ������ �ʿ��� �� �ִ�.  */
		memcpy(&m_OffsetMatrices[i], &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&m_OffsetMatrices[i], XMMatrixTranspose(XMLoadFloat4x4(&m_OffsetMatrices[i])));

		/* �� ���� ��� �ִ� ��ü �����߿� �� �޽ð� ����ϴ� �����̸��� ���� ���� �ε����� �����صд�. */
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(pAIBone->mName.data));


		m_iNumBoneWeight = pAIBone->mNumWeights;

			// pAIBone �� �� �޽��� ������ �ִ� �����߿� �ϳ� !!
			/* �� ���� �(mNumWeights) �������� ������ �ִ°�? */
			for (size_t j = 0; j < pAIBone->mNumWeights; j++)
			{
				/* �� ���� ������ �ִ� j��° ������ �ε���(pAIBone->mWeights[j].mVertexId)*/
				// ���� �� ������ ���� Ư�� ���� ����ġ�� �Ҵ���� �ʾҴٴ� ���� Ȯ���ϱ� ���ؼ��Դϴ�.
				// �� ������ ������ �������ִ� �� 4���� ù��°�� 0�̶�� ������ �������մ°����� �մٴ°Ŵϱ� ! 
				// ������ �ִ� ������ �� �޽��� ������ �ִ� ������ �ϳ��� pAIBone ! �׳༮��  ������ �ִ� �����߿��� ���°? i ��° !
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

	/* �������۸� �����Ѵ�. */
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
