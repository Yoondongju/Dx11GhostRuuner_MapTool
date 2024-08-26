#include "..\Public\BlendObject.h"

#include "../Public/Transform.h"
#include "../Public/GameInstance.h"


CBlendObject::CBlendObject(ID3D11Device*  pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice , pContext }
{
}

CBlendObject::CBlendObject(const CBlendObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlendObject::Initialize(void* pArg)
{
	return S_OK;
}

void CBlendObject::Priority_Update(_float fTimeDelta)
{
}

void CBlendObject::Update(_float fTimeDelta)
{
}

void CBlendObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CBlendObject::Render()
{
	return S_OK;
}

HRESULT CBlendObject::Compute_ViewZ(const _wstring& strTransformComTag)
{
	CTransform* pTransformCom = dynamic_cast<CTransform*>(Find_Component(strTransformComTag));
	if (nullptr == pTransformCom)
		return E_FAIL;

	_vector			vWorldPos = pTransformCom->Get_State(CTransform::STATE_POSITION);

	_matrix		ViewMatrix{};

	ViewMatrix = static_cast<CTransform*>(m_pGameInstance->Find_Camera()->Find_Component(TRANSFORM))->Get_WorldMatrix_Inverse();

	
	

	XMVECTOR ViewPos = XMVector3TransformCoord(vWorldPos, ViewMatrix);

	_float3 vViewPos = {};
	XMStoreFloat3(&vViewPos, ViewPos);

	m_fViewZ = vViewPos.z;

	return S_OK;
}

void CBlendObject::Free()
{
	__super::Free();

}
