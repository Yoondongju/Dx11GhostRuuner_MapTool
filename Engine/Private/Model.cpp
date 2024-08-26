#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"

#include "../Public/Animation.h"
#include "../Public/VIBuffer_Cube.h"

#include <codecvt>

std::wstring charToWstring(const char* pModelFilePath) {
	std::string str(pModelFilePath);
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(str);
}

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_eModelType { Prototype.m_eModelType }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_Meshes{ Prototype.m_Meshes}
	, m_Materials{ Prototype.m_Materials }

	, m_iNumMaterials {Prototype.m_iNumMaterials}
	, m_strModelFilePath {Prototype.m_strModelFilePath }

	//, m_Bones{ Prototype.m_Bones }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }

	, m_iNextAnimIndex {Prototype.m_iNextAnimIndex}
	, m_iCurrentAnimIndex{ Prototype.m_iCurrentAnimIndex }

	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_Animations{ Prototype.m_Animations }

	, m_CurrentTrackPosition{ Prototype.m_CurrentTrackPosition }

	, m_KeyFrameIndices{ Prototype.m_KeyFrameIndices }
{	
	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			m_strMaterials_TexturePath[i][j] = Prototype.m_strMaterials_TexturePath[i][j];
		}		
	}


	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	for (auto& pPrototypeBone : Prototype.m_Bones)		
		m_Bones.emplace_back(pPrototypeBone->Clone());

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_AddRef(pTexture);
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);
}

_uint CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_uint	iBoneIndex = { 0 };
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (0 == strcmp(pBone->Get_Name(), pBoneName))
				return true;
			++iBoneIndex;
			return false;
		});

	if (iter == m_Bones.end())
		MSG_BOX(TEXT("없는데?"));

	return iBoneIndex;
}
 
HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex)
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();


	if (nullptr != m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType])
		return m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName, 0);
	else
		return S_OK;
}

HRESULT CModel::Bind_MeshBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_BoneMatrices(this, pShader, pConstantName);

	return S_OK;
}
HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	_uint		iFlag = { 0 };


	/* 이전 : 모든 메시가 다 원점을 기준으로 그렺니다. */
	/* 이후 : 모델을 구성하는 모든 메시들을 각각 정해진 상태(메시를 배치하기위한 뼈대의 위치에 맞춰서)대로 미리 변환해준다.*/

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	/*aiProcess_GlobalScale*/

	if (eType == TYPE_NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);  // << pModelFilePath 은 내 모델 fbx 를 통해 Scene을 생성한다 .
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_strModelFilePath = charToWstring(pModelFilePath);					  // 모델 경로 넣어주고 

	

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	m_eModelType = eType;

	
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode , -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();
	

	m_Meshes[iMeshIndex]->Get_Cube()->Bind_Buffers();
	m_Meshes[iMeshIndex]->Get_Cube()->Render();

	return S_OK;
}


void CModel::SetUp_Animation(_uint iAnimationIndex, _bool isLoop)
{	
	m_iNextAnimIndex = iAnimationIndex;
 
	m_isLoop = isLoop;
}


_bool CModel::Play_Animation(_float fTimeDelta)
{
	/* 뼈를 움직인다.(CBone`s m_TransformationMatrix행렬을 갱신한다.) */
	_bool isFinished = false;

	_bool isTransitioning = false;
	if (m_iNextAnimIndex != m_iCurrentAnimIndex)
		isTransitioning = true;


	isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, 
		&m_CurrentTrackPosition,
		m_KeyFrameIndices[m_iCurrentAnimIndex],  
		m_isLoop, 
		fTimeDelta, 
		isTransitioning, 
		m_Animations[m_iNextAnimIndex]);
	

	if (m_CurrentTrackPosition == 0.f)	// 다음 애니로 변환 끗!
	{
		m_iCurrentAnimIndex = m_iNextAnimIndex;
	}

	for (auto& pBone : m_Bones)
	{
		/* 내 뼈의 행렬 * 부모의 컴바인드 */
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}


	return isFinished;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, this ,m_eModelType , m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{		
		MESH_MATERIAL		MeshMaterial{};

		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			// pAIMaterial->GetTextureCount(j);
			aiString			strTexturePath;

			_char				szDrive[MAX_PATH] = "";
			_char				szDirectory[MAX_PATH] = "";
			_char				szFileName[MAX_PATH] = "";
			_char				szExt[MAX_PATH] = "";

			_char				szTexturePath[MAX_PATH] = "";

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))	// 18 개의 텍스쳐종류중 만약 없다면 ? 넘어가
				continue;

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szTexturePath, szDrive);
			strcat_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szFileName);
			strcat_s(szTexturePath, szExt);

			_tchar				szFinalPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTexturePath, strlen(szTexturePath), szFinalPath, MAX_PATH);

			m_strMaterials_TexturePath[i][j] = _wstring(szFinalPath);			// 최종 나의 머테리얼의 경로 
				
			MeshMaterial.pMaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFinalPath, 1);
			if (nullptr == MeshMaterial.pMaterialTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAIBone, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pAIBone, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	pBone->Set_NumChildBone(pAIBone->mNumChildren);	// 만약 터지면 여기를봐라 여기가 가장 의심스러움
	m_Bones.emplace_back(pBone);	

	_int		iParentIndex = m_Bones.size() - 1;

	for (size_t i = 0; i < pAIBone->mNumChildren; i++)
	{
		Ready_Bones(pAIBone->mChildren[i], iParentIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

 	m_KeyFrameIndices.resize(m_iNumAnimations);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{		
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_KeyFrameIndices[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();


	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();


	for (auto& Bone : m_Bones)
	{
		Safe_Release(Bone);
	}
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

}
