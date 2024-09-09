#include "stdafx.h"
#include "..\Public\BackPack_Jetpack.h"

#include "Player.h"

#include "GameInstance.h"

CBackPack_Jetpack::CBackPack_Jetpack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBackPack_Jetpack::CBackPack_Jetpack(const CBackPack_Jetpack& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CBackPack_Jetpack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackPack_Jetpack::Initialize(void* pArg)
{
	BACKPACK_DESC* pDesc = static_cast<BACKPACK_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	/* ���������� ���� �����͵��� ��� ���ϳ�. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float4x4 InitWorldMatrix = {
		0.0198405515, 0.0269910656, -0.999438941, 0.00000000,
		0.281111836, 0.959158599, 0.0314840749, 0.00000000,
		0.959471464, -0.281578660, 0.0114423055, 0.00000000,
		-6.04659939, -11.2101269, -0.342136741, 1.00000000
	};
	
	
	m_pTransformCom->Set_WorldMatrix(InitWorldMatrix);

	return S_OK;
}

void CBackPack_Jetpack::Priority_Update(_float fTimeDelta)
{

}

void CBackPack_Jetpack::Update(_float fTimeDelta)
{

}

void CBackPack_Jetpack::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	
	

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBackPack_Jetpack::Render()
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

HRESULT CBackPack_Jetpack::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxMeshTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;




	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Jetpack_Back"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CBackPack_Jetpack* CBackPack_Jetpack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackPack_Jetpack* pInstance = new CBackPack_Jetpack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackPack_Jetpack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBackPack_Jetpack::Clone(void* pArg)
{
	CBackPack_Jetpack* pInstance = new CBackPack_Jetpack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBackPack_Jetpack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackPack_Jetpack::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);


	Safe_Release(m_pModelCom);
}
