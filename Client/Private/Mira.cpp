#include "stdafx.h"
#include "Mira.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Level_MapTool.h"
#include "LandObject.h"

#include "Player.h"

CMira::CMira(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{

}


CMira::CMira(const CMira& Prototype)
    : CMonster(Prototype)
{

}


HRESULT CMira::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CMira::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_pModel->SetUp_Animation(0, true);

    return S_OK;
}

void CMira::Priority_Update(_float fTimeDelta)
{
    

}

void CMira::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);


    m_pModel->Play_Animation(fTimeDelta);
}

void CMira::Late_Update(_float fTimeDelta)
{
    if (m_strLayerName == L"Layer_PreView_Object")
    {
        if (!CLevel_MapTool::IsPicking())        // 피킹한 오브젝트가 있을경우 PireViewObject는 그려주지않는다.
            m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
    }
    else
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

}

HRESULT CMira::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    _float fAlpha = 0.7f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
        return E_FAIL;



    _uint iNumMeshes = m_pModel->Get_MeshesCount();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModel->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CMira::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;





    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Mira"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    return S_OK;
}

CMira* CMira::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMira* pInstance = new CMira(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMira"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMira::Clone(void* pArg)
{
    CMira* pInstance = new CMira(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMira"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMira::Free()
{
    __super::Free();



    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
