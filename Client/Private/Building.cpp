#include "stdafx.h"
#include "Building.h"
#include "GameInstance.h"

#include "Level_MapTool.h"

#include "Terrain.h"

#include "LandObject.h"

CBuilding::CBuilding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CBuilding::CBuilding(const CBuilding& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CBuilding::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBuilding::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;




    return S_OK;
}

void CBuilding::Priority_Update(_float fTimeDelta)
{

}

void CBuilding::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);


    if (m_strLayerName == L"Layer_PreView_Object")
    {
        list<CGameObject*>& TerrainList = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_BackGround_Terrain");        
        if (TerrainList.empty())
            return;
       

        _float fClosestDistance = FLT_MAX;  // 가장 가까운 거리
        CGameObject* pClosestTerrain = nullptr;  // 가장 가까운 지형

        _vector vPreviewPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // PreView 오브젝트의 현재 위치

        for (auto pTerrain : TerrainList)
        {
            _vector vTerrainPosition = pTerrain->Get_Transform()->Get_State(CTransform::STATE_POSITION); // 지형의 좌하단 위치
    
            _vector vDifference = XMVectorSubtract((_fvector)vPreviewPosition, (_fvector)vTerrainPosition);
            

            _float fDistance = XMVectorGetX(XMVector3Length(vDifference));

            if(fDistance < fClosestDistance)
            {
                fClosestDistance = fDistance;
                pClosestTerrain = pTerrain;
            }

        }

        if (pClosestTerrain)
        {
            CLandObject::Set_TerrainVIBuffer(static_cast<CVIBuffer_Terrain*>(pClosestTerrain->Find_Component(L"Com_VIBuffer")));
            CLandObject::Set_TerrainTransform(pClosestTerrain->Get_Transform());

            // 오브젝트를 지형 위로 배치합니다.
            CLandObject::SetUp_OnTerrain(m_pTransformCom, 0.5f);
        }
        
    }

}

void CBuilding::Late_Update(_float fTimeDelta)
{
    if (m_strLayerName == L"Layer_PreView_Object")
    {
        if (!CLevel_MapTool::IsPicking())        // 피킹한 오브젝트가 있을경우 PireViewObject는 그려주지않는다.
            m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
    }
    else
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBuilding::Render()
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
        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }





    return S_OK;
}

HRESULT CBuilding::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxMeshTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    CLevel_MapTool::OBJECT_CHECK_TYPE eType = (CLevel_MapTool::OBJECT_CHECK_TYPE)Get_ObjectType();

    _wstring strPrototypeModelName = L"";

    switch (eType)
    {
    case Client::CLevel_MapTool::CITY:
    {
        switch (m_iModelNum)        // ObjectType은 얜 걍 빌딩이니까 
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_City";
            break;
        default:
            break;
        }
    }
        break;
    case Client::CLevel_MapTool::BUILDING:
    {
        switch (m_iModelNum)        // ObjectType은 얜 걍 빌딩이니까 
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Building0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Building1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Building2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Building3";
            break;
        case 4:
            strPrototypeModelName = L"Prototype_Component_Model_Building4";
            break;
        default:
            break;
        }
    }
        break;
    case Client::CLevel_MapTool::OBJECT_CHECK_TYPE_END:
        break;
    default:
        break;
    }

    





    /* For.Com_Model*/
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, strPrototypeModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    return S_OK;
}



CBuilding* CBuilding::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBuilding* pInstance = new CBuilding(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBuilding"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBuilding::Clone(void* pArg)
{
    CBuilding* pInstance = new CBuilding(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBuilding"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBuilding::Free()
{
    __super::Free();



    m_pRigidStatic->release();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
