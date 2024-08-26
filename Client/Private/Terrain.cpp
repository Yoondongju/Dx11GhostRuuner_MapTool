#include "stdafx.h"
#include "..\Public\Terrain.h"

#include "GameInstance.h"
#include "../Public/Level_MapTool.h"


CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice,pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
	: CGameObject{ Prototype }
	, m_iNumX { Prototype.m_iNumX}
	, m_iNumZ{ Prototype.m_iNumZ }
{
}


HRESULT CTerrain::Initialize_Prototype(_uint iNumX, _uint iNumZ)
{
	m_iNumX = iNumX;
	m_iNumZ = iNumZ;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};
	if (nullptr != pArg)
	{
		Desc = *(CGameObject::GAMEOBJECT_DESC*)pArg;
	}


	__super::Initialize(&Desc);

	if (FAILED(Ready_Components()))
		return E_FAIL;





	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
}

void CTerrain::Update(_float fTimeDelta)
{
	
}

void CTerrain::Late_Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTerrain::Render()	// 이런애들은 렌더러가 호출해줌  
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	 
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))		// 0: 그냥 디퓨즈 컬러   1: 노멀 컬러 로 할예정 ?
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CTerrain::Change_VIBuffer_Terrain(const _wstring& VIBuffer_Terrain_PrototypeName)
{
	if (m_pVIBufferCom)
	{
		Safe_Release(m_pVIBufferCom);
		Safe_Release(m_pVIBufferCom);
	}
	

	wstring prototype = L"Prototype_Component_VIBuffer_Terrain_";

	// prototype 부분 제거 후 남은 문자열 추출
	wstring numX_numZ = VIBuffer_Terrain_PrototypeName.substr(prototype.length());

	// '_'를 기준으로 m_iNumX와 m_iNumZ 분리
	size_t underscorePos = numX_numZ.find(L'_');
	wstring strNumX = numX_numZ.substr(0, underscorePos);
	wstring strNumZ = numX_numZ.substr(underscorePos + 1);

	// wstring을 int로 변환
	m_iNumX = stoi(strNumX);
	m_iNumZ = stoi(strNumZ);


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, VIBuffer_Terrain_PrototypeName,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::Ready_Components()
{

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]), nullptr)))
		return E_FAIL;
	


	_wstring VIBuffer_Terrain_Prototype = L"Prototype_Component_VIBuffer_Terrain_";
	VIBuffer_Terrain_Prototype += (to_wstring(m_iNumX) + L"_" + to_wstring(m_iNumZ));


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, VIBuffer_Terrain_Prototype,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;


	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;
	

	return S_OK;
}



CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , _uint iNumX, _uint iNumZ)
{
	CTerrain* pInstance = new CTerrain(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumX, iNumZ)))
	{
		MSG_BOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pNavigationCom);
}
