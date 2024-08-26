#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* 모델 = 메시 + 메시 + 메시 .. */
/* 메시로 구분하는 이유 : 파츠의 교체를 용이하게 만들어주기 위해서.*/
/* 메시 = 정점버퍼 + 인덱스 */

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

	vector<_uint>& Get_Affected_BoneIndices() { return m_BoneIndices; }		// 여기 잇는 인덱스는 내 모델의 전체뼈에서의 인덱스를 말한다.
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


	_uint				m_iNumBones = { 0 };		// 모델의 전체뼈 갯수가 아닌 !!!!! 이 메쉬에 영향을 주는 뼈의 갯수 !!!!!!!

	_uint				m_iNumBoneWeight = {};		// 해당 뼈는 몇개의 정점에게 영향을 주니 ?


													// m_iNumBones이 어심푸를 통해 받아오는 변수니까 벡터로 햇음 저거랑 크기 맞기위해
	vector<_uint>		m_BoneIndices;				// 내 모델이 들고 잇는 전체뼈중에서 이 메시가 영향을 받는 뼈와 이름이 같은 놈의 인덱스   // 여기서 이름이 같은 놈이란 모델의 전체뼈에 잇는녀석임

	_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};

	vector<_float4x4>	m_OffsetMatrices;


	_float3				m_vMinPos = {};
	_float3				m_vMaxPos = {};




private:
	VTXMESH*			m_pVertices = { nullptr }; // 저장해놔야해
	VTXANIMMESH*		m_pAnimVertices = { nullptr }; // 저장해놔야해

	_uint*				m_pIndices = { nullptr };  // 저장해놔야해


	class CVIBuffer_Cube*		m_pVIBuffer = { nullptr };	// 큐브


private:
	HRESULT		Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT		Ready_VertexBuffer_Anim(const CModel* pModel, const aiMesh* pAIMesh);


public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CModel* pModel , CModel::MODEL_TYPE eType ,const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END