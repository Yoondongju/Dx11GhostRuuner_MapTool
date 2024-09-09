#include "stdafx.h"
#include "..\Public\Weapon_Jetpack.h"

#include "Player.h"

#include "GameInstance.h"

CWeapon_Jetpack::CWeapon_Jetpack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{

}

CWeapon_Jetpack::CWeapon_Jetpack(const CWeapon_Jetpack& Prototype)
	: CPartObject{ Prototype }
{

}

HRESULT CWeapon_Jetpack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Jetpack::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	/* ���������� ���� �����͵��� ��� ���ϳ�. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float4x4 InitWorldMatrix = {
		0.375464141, -0.0646902099, 0.0703297928, 0.00000000,
		-0.0494608544, 0.178602383, 0.428333551, 0.00000000,
		-0.0996612236, -0.406619579, 0.158040211, 0.00000000,
		-1.11516476, -4.24499321, 2.04884958, 1.00000000
	};


	m_pTransformCom->Set_WorldMatrix(InitWorldMatrix);

	return S_OK;
}

void CWeapon_Jetpack::Priority_Update(_float fTimeDelta)
{

}

void CWeapon_Jetpack::Update(_float fTimeDelta)
{

}

void CWeapon_Jetpack::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CWeapon_Jetpack::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_MeshesCount();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Jetpack::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxMeshTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Jetpack_Roket"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CWeapon_Jetpack* CWeapon_Jetpack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Jetpack* pInstance = new CWeapon_Jetpack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Jetpack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Jetpack::Clone(void* pArg)
{
	CWeapon_Jetpack* pInstance = new CWeapon_Jetpack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Jetpack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Jetpack::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pModelCom);
}
