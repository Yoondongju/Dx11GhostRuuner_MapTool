#include "..\Public\Bone.h"

CBone::CBone()
{

}

HRESULT CBone::Initialize(const aiNode* pAIBone , _int iParentBoneIndex)
{
	m_iParentBoneIndex = iParentBoneIndex;

	strcpy_s(m_szName, pAIBone->mName.data);

	memcpy(&m_TransformationMatrix, &pAIBone->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
	//  최종 내 컴바인드 == 내 TransformMatrix * 부모의 컴바인드 (이때 PreTransformMatrix 안곱해주는이유는 이미 최상위 루트 부모에서 햇음)

}

CBone* CBone::Create(const aiNode* pAIBone , _int iParentBoneIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAIBone , iParentBoneIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}


void CBone::Free()
{
	__super::Free();

}
