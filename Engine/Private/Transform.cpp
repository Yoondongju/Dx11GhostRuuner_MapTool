#include "../Public/Transform.h"
#include "../Public/Shader.h"

#include "Navigation.h"

CTransform::CTransform(ID3D11Device*  pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice,pContext)
{
}

CTransform::CTransform(const CTransform& Prototype)
	: CComponent(Prototype)
	, m_WorldMatrix(Prototype.m_WorldMatrix)
{

}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	XMStoreFloat4x4(&m_WorldMatrix , pDesc->InitWorldMatrix);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fOriginSpeed = m_fSpeedPerSec;

	m_fRotationPerSec = pDesc->fRotationPerSec;

	m_eComponentType = TRANSFORM;

	m_fOriginPosY = XMVectorGetY(Get_State(STATE::STATE_POSITION));

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}



void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fScaleZ);
}

void CTransform::Go_Straight_TargetObject(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fOriginSpeed * fTimeDelta;

	vPosition += vCompute_Result;


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight(_float fTimeDelta , class CNavigation* pNavigation)
{
	m_fOriginPosY = 10;

	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);


	
	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec* fTimeDelta;

	vPosition += vCompute_Result;

	

	_float fPosY = {};

	Set_State(STATE_POSITION, vPosition);

	//if (nullptr == pNavigation || true == pNavigation->isMove(vPosition , &fPosY))
	//{
	//	if (pNavigation)
	//		vPosition = XMVectorSetY(vPosition, m_fOriginPosY + fPosY);
	//
	//	
	//}
		
}

void CTransform::Go_Backward(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	

	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition -= vCompute_Result;
	


	_float fPosY = {};

	Set_State(STATE_POSITION, vPosition);

	//if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, &fPosY))
	//{
	//	if (pNavigation)
	//		vPosition = XMVectorSetY(vPosition, m_fOriginPosY + fPosY);
	//
	//	
	//}
}

void CTransform::Go_Right(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_RIGHT);


	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition += vCompute_Result;
	


	_float fPosY = {};

	Set_State(STATE_POSITION, vPosition);

	//if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, &fPosY))
	//{
	//	if (pNavigation)
	//		vPosition = XMVectorSetY(vPosition, m_fOriginPosY + fPosY);
	//
	//	
	//}
}

void CTransform::Go_Left(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_RIGHT);

	

	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition -= vCompute_Result;



	_float fPosY = {};

	Set_State(STATE_POSITION, vPosition);

	//if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, &fPosY))
	//{
	//	if (pNavigation)
	//		vPosition = XMVectorSetY(vPosition, m_fOriginPosY + fPosY);
	//
	//	
	//}
}

void CTransform::Turn(const _fvector& vAxis, _float fTimeDelta, _float4x4* RotationMatrix)
{
	// �̰� ���� ������ǥ������ ����  ���� �� ������ǥ�� ��ġ���� ��
	_vector vRight = Get_State(STATE_RIGHT);				// ���� ���� �մ� ������� ������ ����
	_vector	vUp = Get_State(STATE_UP);
	_vector	vLook = Get_State(STATE_LOOK);

	
	// �����ӿ����� ȸ�� ������Ʈ
	_matrix vRotationMatrix = XMMatrixRotationAxis(vAxis , m_fRotationPerSec * fTimeDelta);

	// ���� ȸ�� ��� ���
	if(RotationMatrix)
		XMStoreFloat4x4(RotationMatrix , XMMatrixMultiply(XMLoadFloat4x4(RotationMatrix), vRotationMatrix));



	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, vRotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, vRotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, vRotationMatrix));
}


void CTransform::Turn(const _fmatrix& vRotationMatrix)
{
	_vector vRight = Get_State(STATE_RIGHT);				// ���� ���� �մ� ������� ������ ����
	_vector	vUp = Get_State(STATE_UP);
	_vector	vLook = Get_State(STATE_LOOK);




	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, vRotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, vRotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, vRotationMatrix));
}










void CTransform::Rotation(const _fvector& vAxis, _float fRadian)	// 1ȸ���̳� �ڱⰡ ȸ���Ǳ� ���� ó�� ��������
{
	// �׵�� �����Ѱ�����ŭ ����
	_float3 vScaled = Get_Scaled();							
	
	_vector vRight = _vector{ 1.f,0.f,0.f };
	_vector vUp = _vector{ 0.0f,1.f,0.f };
	_vector vLook = _vector{ 0.0f,0.f,1.f };



	_float4x4 RotationMatrix {};

	_matrix vRotationMatrix = XMMatrixRotationAxis(vAxis , fRadian);

	// ȸ���� ���� �������� �ޱ��� ���� 1.f,1.f,1.f���� ȸ����Ű�� ������÷�

	XMVector3TransformNormal(vRight, vRotationMatrix);
	XMVector3TransformNormal(vUp, vRotationMatrix);
	XMVector3TransformNormal(vLook, vRotationMatrix);


	Set_State(STATE_RIGHT, vRight * vScaled.x);
	Set_State(STATE_UP, vUp * vScaled.y);
	Set_State(STATE_LOOK, vLook * vScaled.z);
}


void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}


void CTransform::LookAt(const _fvector& vAt) 
{
	_float3 vScaled = Get_Scaled();

	_vector vPosition = Get_State(STATE_POSITION);


	
	_vector vLook = vAt - vPosition;
	_vector vRight{}, vUp{};
	

	
	
	//_float3 vMyup = Get_State(STATE_UP);
	//D3DXVec3Cross(&vRight, &vMyup, &vLook);// << ������ �����Ͱ� 0,1,0�� �ƴ϶� ������ �����ͷ� �ϸ�?
	// Ȯ���غ����� �����ذ� X
	
	_float3 vUpSample = { 0.f, 1.f, 0.f };

	vRight = XMVector3Cross(XMLoadFloat3(&vUpSample), vLook);
	vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

CTransform* CTransform::Create(ID3D11Device*  pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTransform* pInstance = new CTransform(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTransform::Free()
{
	__super::Free();


}
