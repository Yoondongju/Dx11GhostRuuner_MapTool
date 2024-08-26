#include "stdafx.h"
#include "Aid_props.h"
#include "GameInstance.h"

#include "Level_MapTool.h"

#include "Terrain.h"

#include "LandObject.h"

CAid_props::CAid_props(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CAid_props::CAid_props(const CAid_props& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CAid_props::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAid_props::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

 

    return S_OK;
}

void CAid_props::Priority_Update(_float fTimeDelta)
{

}

void CAid_props::Update(_float fTimeDelta)
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

            if (fDistance < fClosestDistance)
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

void CAid_props::Late_Update(_float fTimeDelta)
{
    if (m_strLayerName == L"Layer_PreView_Object")   
    {
        if(!CLevel_MapTool::IsPicking())        // 피킹한 오브젝트가 있을경우 PireViewObject는 그려주지않는다.
            m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
    }      
    else
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CAid_props::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    _float fAlpha = 0.7f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha",&fAlpha, sizeof(_float))))
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

HRESULT CAid_props::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxMeshTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    CLevel_MapTool::OBJECT_CHECK_TYPE eType = (CLevel_MapTool::OBJECT_CHECK_TYPE)Get_ObjectType();

    _wstring strPrototypeModelName = L"";

    switch (eType)
    {
    case Client::CLevel_MapTool::KIT:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Kit0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Kit1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Kit2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Kit3";
            break;
        case 4:
            strPrototypeModelName = L"Prototype_Component_Model_Kit4";
            break;

        default:
            break;
        }
    }
        break;
    case Client::CLevel_MapTool::DEBRIS:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Debris0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Debris1";
            break;

        default:
            break;
        }
    }
        break;
    case Client::CLevel_MapTool::SIGN:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Sign0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Sign1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Sign2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Sign3";
            break;
        case 4:
            strPrototypeModelName = L"Prototype_Component_Model_Sign4";
            break;
        case 5:
            strPrototypeModelName = L"Prototype_Component_Model_Sign5";
            break;
        }
    }
        break;
    case Client::CLevel_MapTool::WALL:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Wall0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Wall1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Wall2";
            break;
        }
    }
        break;
    case Client::CLevel_MapTool::RUSSIAN_SIGN:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign3";
            break;
        case 4:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign4";
            break;
        case 5:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign5";
            break;
        case 6:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign6";
            break;
        case 7:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign7";
            break;
        case 8:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign8";
            break;
        case 9:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign9";
            break;
        case 10:
            strPrototypeModelName = L"Prototype_Component_Model_Russian_Sign10";
            break;
        }
    }
    break;

    case Client::CLevel_MapTool::ETC:
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

CAid_props* CAid_props::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAid_props* pInstance = new CAid_props(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAid_props"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAid_props::Clone(void* pArg)
{
    CAid_props* pInstance = new CAid_props(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAid_props"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAid_props::Free()
{
    __super::Free();


    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
