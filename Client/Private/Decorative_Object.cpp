#include "stdafx.h"
#include "Decorative_Object.h"
#include "GameInstance.h"

#include "Level_MapTool.h"

#include "Terrain.h"

#include "LandObject.h"

CDecorative_Object::CDecorative_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CDecorative_Object::CDecorative_Object(const CDecorative_Object& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CDecorative_Object::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDecorative_Object::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

 

    return S_OK;
}

void CDecorative_Object::Priority_Update(_float fTimeDelta)
{

}

void CDecorative_Object::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
 
   
    

}

void CDecorative_Object::Late_Update(_float fTimeDelta)
{
    if (m_strLayerName == L"Layer_PreView_Object")   
    {
        if(!CLevel_MapTool::IsPicking())        // ÇÇÅ·ÇÑ ¿ÀºêÁ§Æ®°¡ ÀÖÀ»°æ¿ì PireViewObject´Â ±×·ÁÁÖÁö¾Ê´Â´Ù.
            m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
    }      
    else
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CDecorative_Object::Render()
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

HRESULT CDecorative_Object::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxMeshTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    CLevel_MapTool::MODEL_CHECK_LIST eType = (CLevel_MapTool::MODEL_CHECK_LIST)Get_ModelListType();

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
    case Client::CLevel_MapTool::BARRICADE:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Barricade0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Barricade1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Barricade2";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::BILLBOARD:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard3";
            break;
        case 4:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard4";
            break;
        case 5:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard5";
            break;
        case 6:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard6";
            break;
        case 7:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard7";
            break;
        case 8:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard8";
            break;
        case 9:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard9";
            break;
        case 10:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard10";
            break;
        case 11:
            strPrototypeModelName = L"Prototype_Component_Model_Billboard11";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::BOSS_FIGHT1:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight3";
            break;
        case 4:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight4";
            break;
        case 5:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight5";
            break;
        case 6:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight6";
            break;
        case 7:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight7";
            break;
        case 8:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight8";
            break;
        case 9:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight9";
            break;
        case 10:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight10";
            break;
        case 11:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight11";
            break;
        case 12:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight12";
            break;
        case 13:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight13";
            break;
        case 14:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight14";
            break;
        case 15:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight15";
            break;
        case 16:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight16";
            break;
        case 17:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight17";
            break;
        case 18:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight18";
            break;
        case 19:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight19";
            break;
        case 20:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight20";
            break;

        default:
            break;
        }
        break;
    case Client::CLevel_MapTool::BOSS_FIGHT3:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight3_0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Boss_Fight3_1";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::CATWALK:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_CatWalk0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_CatWalk1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_CatWalk2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_CatWalk3";
            break;
        case 4:
            strPrototypeModelName = L"Prototype_Component_Model_CatWalk4";
            break;
        case 5:
            strPrototypeModelName = L"Prototype_Component_Model_CatWalk5";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::CRANE:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Crane0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Crane1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Crane2";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::CYBERCITY:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_CyberCity0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_CyberCity1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_CyberCity2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_CyberCity3";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::DECO:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Deco0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Deco1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Deco2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Deco3";
            break;
        case 4:
            strPrototypeModelName = L"Prototype_Component_Model_Deco4";
            break;
        case 5:
            strPrototypeModelName = L"Prototype_Component_Model_Deco5";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::FAN:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Fan0";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::INDUSTRIAL_ELEVATOR:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Industrial_Elevator0";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::LAMP:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Lamp0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Lamp1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Lamp2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Lamp3";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::PIPE:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Pipe0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Pipe1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Pipe2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Pipe3";
            break;
        case 4:
            strPrototypeModelName = L"Prototype_Component_Model_Pipe4";
            break;
        case 5:
            strPrototypeModelName = L"Prototype_Component_Model_Pipe5";
            break;
        case 6:
            strPrototypeModelName = L"Prototype_Component_Model_Pipe6";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::PLATFORM:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Platform0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Platform1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Platform2";
            break;
        case 3:
            strPrototypeModelName = L"Prototype_Component_Model_Platform3";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::ROTATIONFAN:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_RotationFan0";
            break;

        default:
            break;
        }
    }
    break;
    case Client::CLevel_MapTool::TRAIN:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Train0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Train1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Train2";
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
    case Client::CLevel_MapTool::CITY:
    {
        switch (m_iModelNum)        // ObjectTypeÀº ¾ë °Á ºôµùÀÌ´Ï±î 
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
        switch (m_iModelNum)        // ObjectTypeÀº ¾ë °Á ºôµùÀÌ´Ï±î 
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
    case Client::CLevel_MapTool::WIRE:
    {
        switch (m_iModelNum)        // ObjectTypeÀº ¾ë °Á ºôµùÀÌ´Ï±î 
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_Wire0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_Wire1";
            break;
        case 2:
            strPrototypeModelName = L"Prototype_Component_Model_Wire2";
            break;
        }
    }
    break;

    case Client::CLevel_MapTool::BODYBAG:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_BodyBag0";
            break;
        case 1:
            strPrototypeModelName = L"Prototype_Component_Model_BodyBag1";
            break;
        default:
            break;
        }
    }
    break;

    case Client::CLevel_MapTool::CLIMBOBJECT:
    {
        switch (m_iModelNum)
        {
        case 0:
            strPrototypeModelName = L"Prototype_Component_Model_ClimbObject0";
            break;
        default:
            break;
        }
    }
    break;





    default:
        break;
    }

    }


    /* For.Com_Model*/
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, strPrototypeModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    return S_OK;

    
}

CDecorative_Object* CDecorative_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDecorative_Object* pInstance = new CDecorative_Object(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDecorative_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDecorative_Object::Clone(void* pArg)
{
    CDecorative_Object* pInstance = new CDecorative_Object(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDecorative_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDecorative_Object::Free()
{
    __super::Free();


    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
