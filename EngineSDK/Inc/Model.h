#pragma once

#include "Component.h"
#include "Bone.h"


BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODEL_TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END};

private:
	CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_float4x4				Get_PreTransformMatrix() { return m_PreTransformMatrix; }

	_uint					Get_MeshesCount() { return m_iNumMeshes; }
	vector<class CMesh*>&	Get_Meshes() { return m_Meshes; }

	MODEL_TYPE				Get_ModelType() { return m_eModelType; }


	_uint Get_BoneIndex(const _char* pBoneName) const;

	_matrix Get_BoneCombindTransformationMatrix(_uint iBoneIndex) const {
		return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
	}

	const _float4x4* Get_BoneCombindTransformationMatrix_Ptr(const _char* pBoneName) const {
		return m_Bones[Get_BoneIndex(pBoneName)]->Get_CombinedTransformationMatrix_Ptr();
	}


	vector<class CBone*>& Get_Bones() { return m_Bones; }
	

	const _wstring&			Get_m_strModelFilePath() { return m_strModelFilePath; }

	_uint					Get_MaterialsCount() { return m_iNumMaterials; }

	_wstring* 				Get_MaterialTexturePath(_uint iMaterial) { return m_strMaterials_TexturePath[iMaterial]; }


	vector<class CAnimation*>& Get_Animations() { return m_Animations; }
	class CAnimation* Get_CurAnimation() { return m_Animations[m_iCurrentAnimIndex]; }
	_double&	      Get_Referene_CurrentTrackPosition() { return m_CurrentTrackPosition; }

	_uint			  Get_NumAnimations() { return m_iNumAnimations; }
	_uint			  Get_CurAnimationIndex() { return m_iCurrentAnimIndex; }

	vector<vector<_uint>>&  Get_KeyFrameIndices() { return m_KeyFrameIndices; }


public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex);
	HRESULT Bind_MeshBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);


public:
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eType , const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void SetUp_Animation(_uint iAnimationIndex, _bool isLoop = false);


	/* ���� �����δ�.(CBone`s m_TransformationMatrix����� �����Ѵ�.) */
	/* ��� ���� ������ �ִ� m_CombinedTransformationMatrix�� �����Ѵ�. */
	_bool Play_Animation(_float fTimeDelta);

private:
	/* ���� �־��� ��ο� �ش��ϴ� ������ ������ �о aiScene��ü�� �������ش�. */
	Assimp::Importer				m_Importer;

	/* ���Ϸκ��� �о�� ��� ������ �����ϰ� �ִ´�. */
	const aiScene*		m_pAIScene = { nullptr };
	MODEL_TYPE			m_eModelType = { TYPE_END };


private: /* �޽��� ������ �����Ѵ�. */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;
	_float4x4						m_PreTransformMatrix = {};

private:
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>			m_Materials;

private:
	vector<class CBone*>			m_Bones;						// �� ���� ��ü ��

private:
	_bool							m_isLoop = { false };			// �ִ� �ݺ�����

	
	_uint							m_iNextAnimIndex = { 0 };		// ���� �ִϸ��̼� �ε���
	_uint							m_iCurrentAnimIndex = { 0 };	// ���� �� �ִ� �ε���


	_uint							m_iNumAnimations = { 0 };		// �� �ִϸ��̼� �Ѱ���
	vector<class CAnimation*>		m_Animations;					// �� ���� �ִϸ��̼�


	_double							m_CurrentTrackPosition = {};    /* ���� �ִϸ��̼��� ��� ��ġ. */


	vector<vector<_uint>>			m_KeyFrameIndices;				// � �ִϸ��̼��� Ű������ �ε�����  	/* �� �ִϸ��̼Ǵ� �� ä�ε��� ���� Ű�������ε��� */



private:
	_wstring						m_strModelFilePath = L"";
	_wstring						m_strMaterials_TexturePath[15][AI_TEXTURE_TYPE_MAX] = { L"" };




public:
	HRESULT	Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAIBone, _int iParentBoneIndex);
	HRESULT Ready_Animation();

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, MODEL_TYPE eType,const _char * pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END