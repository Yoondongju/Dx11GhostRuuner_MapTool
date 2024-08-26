#include "stdafx.h"
#include "..\Public\LandObject.h"

#include "GameInstance.h"


CVIBuffer_Terrain* CLandObject::m_pTerrainVIBuffer = nullptr; 
CTransform* CLandObject::m_pTerrainTransform = nullptr;


CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice,pContext}
{
}

CLandObject::CLandObject(const CLandObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	



	return S_OK;
}

void CLandObject::Priority_Update(_float fTimeDelta)
{
}

void CLandObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	

}

void CLandObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

HRESULT CLandObject::SetUp_OnTerrain(CTransform* pTransform, _float fOffsetY)
{
	/* ������ Ÿ���ϴ� ��ü�� ���� ��ġ�� ���´�. */
	_vector		vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);

	/* ���� ������ �� ��, ���ý����̽����� ���� �䷮�̱�Ť̤��� ��ü�� ��ġ�� ���������� ���÷� ��ȯ��Ű��. */
	/* ��ü�� ������ġ * �������� �׸��� ��ü�� ���� ����� */
	_matrix		WorldMatrixInverse = m_pTerrainTransform->Get_WorldMatrix_Inverse();
	_matrix     WorldMatrix = m_pTerrainTransform->Get_WorldMatrix();


	_vector		vLocalPos{};
	vLocalPos = XMVector3TransformCoord(vWorldPos, WorldMatrixInverse);





	_float		fHeight = m_pTerrainVIBuffer->Compute_Height(vLocalPos);

	_float ff = fHeight + (fOffsetY * pTransform->Get_Scaled().y);

	vLocalPos = XMVectorSetY(vLocalPos, fHeight + (fOffsetY * pTransform->Get_Scaled().y));// << �� �����Ͽ� �°� y ��ġ����
   
	/* ������ ���� �����̽� �� ���ŵ� ������ġ�� ���Ѱ��̱⶧���� */
	/* �̸� �ٽ� ����� ������(���������� �����͸� -> ���� )���ؼ��� ��������� ���ؾ��Ѵ�. */

	vWorldPos = XMVector3TransformCoord(vLocalPos , WorldMatrix);

	pTransform->Set_State(CTransform::STATE_POSITION, vWorldPos);

	return S_OK;
}



void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pTerrainVIBuffer);
	Safe_Release(m_pTerrainTransform);
}
