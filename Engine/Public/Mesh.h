#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* �� = �޽� + �޽� + �޽� .. */
/* �޽÷� �����ϴ� ���� : ������ ��ü�� �����ϰ� ������ֱ� ���ؼ�.*/
/* �޽� = �������� + �ε��� */

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}

	_uint Get_MaterialIndex() const
	{
		return m_iMaterialIndex;
	}

	_uint Get_Num_Affected_Bones() const
	{
		return m_iNumBones;
	}

	_uint Get_Num_Affected_Bone_Weight() const
	{
		return m_iNumBoneWeight;
	}

	vector<_uint>& Get_Affected_BoneIndices() { return m_BoneIndices; }		// ���� �մ� �ε����� �� ���� ��ü�������� �ε����� ���Ѵ�.
	vector<_float4x4>& Get_OffsetMatrices() { return m_OffsetMatrices; }



	
public:
	VTXMESH*         Get_Vertices() { return m_pVertices; }
	VTXANIMMESH*	 Get_AnimVertices() { return m_pAnimVertices; }


	_uint*			 Get_Indices() { return m_pIndices; }

	 _float3   Get_MinPos() const { return m_vMinPos; }
	 _float3   Get_MaxPos() const { return m_vMaxPos; }

	 class CVIBuffer_Cube* Get_Cube() { return m_pVIBuffer; }



public:
	virtual HRESULT Initialize_Prototype(const CModel* pModel, CModel::MODEL_TYPE eType  ,const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;


public:
	HRESULT Bind_BoneMatrices(const CModel* pModel, class CShader* pShader, const _char* pConstantName);


public:
	virtual _bool isPicking(const _fmatrix& WorldMatrix, _float3* pOut) override;



private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iMaterialIndex = { 0 };


	_uint				m_iNumBones = { 0 };		// ���� ��ü�� ������ �ƴ� !!!!! �� �޽��� ������ �ִ� ���� ���� !!!!!!!

	_uint				m_iNumBoneWeight = {};		// �ش� ���� ��� �������� ������ �ִ� ?


													// m_iNumBones�� ���Ǫ�� ���� �޾ƿ��� �����ϱ� ���ͷ� ���� ���Ŷ� ũ�� �±�����
	vector<_uint>		m_BoneIndices;				// �� ���� ��� �մ� ��ü���߿��� �� �޽ð� ������ �޴� ���� �̸��� ���� ���� �ε���   // ���⼭ �̸��� ���� ���̶� ���� ��ü���� �մ³༮��

	_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};

	vector<_float4x4>	m_OffsetMatrices;


	_float3				m_vMinPos = {};
	_float3				m_vMaxPos = {};




private:
	VTXMESH*			m_pVertices = { nullptr }; // �����س�����
	VTXANIMMESH*		m_pAnimVertices = { nullptr }; // �����س�����

	_uint*				m_pIndices = { nullptr };  // �����س�����


	class CVIBuffer_Cube*		m_pVIBuffer = { nullptr };	// ť��


private:
	HRESULT		Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT		Ready_VertexBuffer_Anim(const CModel* pModel, const aiMesh* pAIMesh);


public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CModel* pModel , CModel::MODEL_TYPE eType ,const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END