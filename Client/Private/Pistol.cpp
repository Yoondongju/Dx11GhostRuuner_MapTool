#include "stdafx.h"
#include "Pistol.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Level_MapTool.h"

#include "Weapon_Pistol.h"

CPistol::CPistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
{

}


CPistol::CPistol(const CPistol& Prototype)
    : CContainerObject(Prototype)
{

}


HRESULT CPistol::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CPistol::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Parts()))
        return E_FAIL;
    

    m_pModel->SetUp_Animation(0, true);

    return S_OK;
}

void CPistol::Priority_Update(_float fTimeDelta)
{
    
    for (auto& pPartObject : m_Parts)
        pPartObject->Priority_Update(fTimeDelta);
}

void CPistol::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);


    m_pModel->Play_Animation(fTimeDelta);

    for (auto& pPartObject : m_Parts)
        pPartObject->Update(fTimeDelta);
}

void CPistol::Late_Update(_float fTimeDelta)
{
    if (m_strLayerName == L"Layer_PreView_Object")
    {
        if (!CLevel_MapTool::IsPicking())        // 피킹한 오브젝트가 있을경우 PireViewObject는 그려주지않는다.
            m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
    }
    else
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

    for (auto& pPartObject : m_Parts)
        pPartObject->Late_Update(fTimeDelta);

}

HRESULT CPistol::Render()
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

HRESULT CPistol::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;





    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Pistol"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CPistol::Ready_Parts()
{
    m_Parts.resize(PART_END);


    CWeapon_Pistol::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("Gun_r");
    WeaponDesc.pOwner = this;
    WeaponDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon_Pistol"), &WeaponDesc)))
        return E_FAIL;

    return S_OK;
}

CPistol* CPistol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPistol* pInstance = new CPistol(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPistol"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPistol::Clone(void* pArg)
{
    CPistol* pInstance = new CPistol(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CPistol"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPistol::Free()
{
    __super::Free();



    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
