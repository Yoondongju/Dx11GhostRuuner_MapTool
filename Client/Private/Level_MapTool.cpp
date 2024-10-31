#include "stdafx.h"
#include "../Public/Level_MapTool.h"
#include "../Public/Terrain.h"
#include "FreeCamera.h"

#include "GameInstance.h"
#include "Cell.h"

#include "GameObject.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include "Model.h"
#include "Mesh.h"

#include "Bone.h"
#include "Animation.h"
#include "Channel.h"

#include "Decorative_Object.h"
#include "Static_Object.h"
#include "Dynamic_Object.h"

#include "Spider.h"


#include "Player.h"
#include "PartObject.h"
#include "Weapon_Player.h"

#include "Jetpack.h"
#include "Pistol.h"
#include "Sniper.h"

#include <codecvt>
#pragma warning (disable : 4996)

static ImGuizmo::OPERATION eGizmoType = { ImGuizmo::TRANSLATE };
static CLevel_MapTool::OBJECT_TYPE eObjectType = { CLevel_MapTool::OBJECT_TYPE::STATIC_OBJECT };

CGameObject* CLevel_MapTool::m_pPickedObj = nullptr;

std::istream& operator>>(std::istream& is, _float4x4& mat) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            is >> mat.m[i][j];
        }
    }
    return is;
}

std::string wstringToString(const std::wstring& wstr) {
    // Note: std::wstring_convert is deprecated in C++17 and removed in C++20.
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

std::wstring stringToWstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}


_bool g_PickingCity = false;

_bool g_PickingTerrain = false;
_bool g_FollowPlayer = false;

_bool g_PlayerAnimationControler = false;
_bool g_NavigationOn = false;

CLevel_MapTool::CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice,pContext }
{
}

HRESULT CLevel_MapTool::Initialize()
{
    if (FAILED(Ready_Light()))
        return E_FAIL;

    if (FAILED(Ready_Camera()))
        return E_FAIL;

    if (FAILED(Ready_ImGui()))         // IMGUI
        return E_FAIL;



    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.fSpeedPerSec = 30.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    //if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"),&Desc)))
	//	return E_FAIL;
    
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"),&Desc)))
       assert(nullptr);




    return S_OK;
}


HRESULT CLevel_MapTool::Ready_Light()
{
    LIGHT_DESC			LightDesc{};

    ZeroMemory(&LightDesc, sizeof LightDesc);
    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    ZeroMemory(&LightDesc, sizeof LightDesc);
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 1.f);
    LightDesc.fRange = 7.f;
    LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
    LightDesc.vAmbient = /*_float4(0.4f, 0.2f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    ZeroMemory(&LightDesc, sizeof LightDesc);
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(20.f, 3.f, 10.f, 1.f);
    LightDesc.fRange = 7.f;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = /*_float4(0.2f, 0.4f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;
}

HRESULT CLevel_MapTool::Ready_ImGui()
{
    // �����κ�
    ImGui::CreateContext();          // Create ������ ���� ������� , �갡 ������ ����������
    ImGui::StyleColorsDark();        // Style -> ����
 
    
    // ImGuizmo Ȱ��ȭ
    ImGuizmo::Enable(true); 

    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig font_cfg;
    font_cfg.OversampleH = 2;
    font_cfg.OversampleV = 2;
    font_cfg.PixelSnapH = true;

    // ��Ʈ ���� ��θ� �����ϰ� �����ϼ���
    io.Fonts->AddFontFromFileTTF("C://Windows//Fonts//gulim.ttc", 14.0f, nullptr, io.Fonts->GetGlyphRangesKorean());


    if (!ImGui_ImplWin32_Init(g_hWnd))  // win32 ���õ� �ʱ�ȭ
    {
        MSG_BOX(L"IMGUI WIN32 �ʱ�ȭ ����");
        return E_FAIL;
    }

    if (!ImGui_ImplDX11_Init(m_pDevice,m_pContext))  // Dx ���õ� �ʱ�ȭ
    {
        MSG_BOX(L"IMGUI DX11 �ʱ�ȭ ����");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_MapTool::Ready_Camera()
{
    CFreeCamera::CAMERA_FREE_DESC		Desc{};

    Desc.fSensor = 0.2f;
    Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
    Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.fFovy = XMConvertToRadians(90.0f);
    Desc.fNear = 0.1f;
    Desc.fFar = 4000.f;
    Desc.fSpeedPerSec = 30.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
        return E_FAIL;

    m_pFreeCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera());
    Safe_AddRef(m_pFreeCamera);

    return S_OK;
}


// ���� ���� ���� �̸����Ⱑ �ʿ��ϰ�  ( ���� �׸��� ���۸� �� �����? ) �ʹ� ��ȿ���� �ϴ� ���� �̰�

// ����Ʈ �ֺ��� Ŭ�������� �� �ֺ��� ��ġ�� ���� ������������ ����������ϰ�

// ����Ʈ�� ���̸� �����ϴ� ����� �ʿ��ϴ�  O


void CLevel_MapTool::Update(_float fTimeDelta)
{
    if (true == g_NavigationOn && nullptr != m_pPickedObj)
    {              
        vector<CCell*>& Cells = m_pPickedObj->Get_Navigation()->Get_Cells();    // ���� ����� �߰��ѽ����� �ͷ����� �׺� -> ����
        vector<CCell*>& FreeCameraCells = m_pFreeCamera->Get_Navigation()->Get_Cells();

        m_iCellIndex = Cells.size();

        if (false == g_bFocusImGui && m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
        {
            m_CellPoints[m_iPointIndex++ % 3] = m_pGameInstance->Get_PickingBackGround_JustPos(m_fMouseY);   // y���� ?
            
           
            

                    
            // ������ �׻� �̾������� ? �� �̾����� ? �ű⼭ ���� �����.
            // ���� �ؾ������� ����? ���߿��� ���� ��� ���� ����Ʈ�� ���� �����Ѱ����� �����ִ°��̴�.
            // m_CellPoints�̰� ������� ���� ��ġ��
            
            if (m_iPointIndex % 3 == 1 || m_iPointIndex % 3 == 0)
            {
                _vector vLength = {};
                _vector vMinLength = {};
                _uint iCellIndex, iPointIndex = 0;

                for (_uint i = 0; i < Cells.size(); ++i)
                {                 
                    for (_uint j = 0; j < CCell::POINT_END; j++)
                    {
                                          
                        vLength = XMVector3Length(XMVectorSubtract(XMLoadFloat3(&m_CellPoints[(m_iPointIndex - 1) % 3]), Cells[i]->Get_Point(CCell::POINT(j))));

                        if (i == 0 && j == CCell::POINT_A)
                            vMinLength = vLength;


                        vMinLength = XMVectorMin(vMinLength, vLength);  

                        if(XMVector3Equal(vMinLength ,vLength))        // �ּ� ���̰� ���� �������� ���Ǽ� ��ġ�� ���̰� ���ٸ�
                        {
                            iCellIndex = i;
                            iPointIndex = j;
                        }
                    }                             
                }


                if (Cells.size() > 0)
                {
                    if (XMVectorGetX(vMinLength) > 5.f)
                    {
                        m_CellPoints[(m_iPointIndex - 1) % 3] = { 0.f,0.f,0.f };

                        MSG_BOX(L"������ ������Ʈ�� ã�� ���߽��ϴ�\n 5 ������ �Ÿ��ȿ��� ��������");
                        m_iPointIndex--;
                        return;
                    }
                    else
                    {               
                        XMStoreFloat3(&m_CellPoints[(m_iPointIndex - 1) % 3], Cells[iCellIndex]->Get_Point(CCell::POINT(iPointIndex)));
                    }
                        
                }
              
            }
            


            if (m_iPointIndex % 3 == 0)      // 3�� ����� �ɶ�����
            {
                // ���� �ٵ� �ݽð�� �������� �ε����� �����ؾ����� 
                // A,B,C       A == 0 , B == 1   C == 2

                // AB   // AC                   
                _vector vCrossVector = XMVector3Cross(XMVectorSubtract(XMLoadFloat3(&m_CellPoints[1]), XMLoadFloat3(&m_CellPoints[0])),
                    XMVectorSubtract(XMLoadFloat3(&m_CellPoints[2]), XMLoadFloat3(&m_CellPoints[0])));

                _vector CrossDirNormalize = XMVector3Normalize(vCrossVector);


                // ī�޶��� ���⺤��
                _vector CamDirNormalize = XMVector3Normalize(m_pFreeCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK));


                _float fDotProduct = XMVectorGetX(XMVector3Dot(CrossDirNormalize, CamDirNormalize));

                if (0.f < fDotProduct)  // �ݽð�� ������?
                {
                    // �ι�° ���� ���������� ������
                    swap(m_CellPoints[1], m_CellPoints[2]);
                }
                                                             
                
                CCell* pCell = CCell::Create(m_pDevice, m_pContext, m_CellPoints, m_iCellIndex);
                if (nullptr == pCell)
                    MSG_BOX(L"�� ���� ����");

                // � ��ü�� � �׺� ?
                               
                Cells.emplace_back(pCell);
                FreeCameraCells.emplace_back(pCell);


                ZeroMemory(m_CellPoints, sizeof(_float3) * 3);
            }  

        }     
    }

    
    //if (m_pPickedObj && m_pGameInstance->Get_KeyState(KEY::CTRL) == KEY_STATE::HOLD)
    //{
    //    if (m_pGameInstance->Get_KeyState(KEY::Z) == KEY_STATE::TAP)
    //    {
    //        vector<CCell*>& Cells = m_pPickedObj->Get_Navigation()->Get_Cells();
    //        vector<CCell*>& FreeCameraCells = m_pFreeCamera->Get_Navigation()->Get_Cells();
    //
    //        if (Cells.size() > 0)
    //        {
    //            Safe_Release(Cells.back());
    //            Safe_Release(FreeCameraCells.back());
    //
    //            Cells.pop_back();
    //            FreeCameraCells.pop_back();
    //
    //            if(m_iCellIndex > 0)
    //                --m_iCellIndex;
    //        }         
    //    }
    //}





    if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
    {
        if (nullptr != m_tPreViewObject.pTargetObject)
        {
            if (FAILED(Create_Object()))
                MSG_BOX(L"������Ʈ ���� ����");
        }
    }

    
    if (m_pGameInstance->Get_KeyState(KEY::ALT) == KEY_STATE::HOLD && m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
    {      
        if (IsPicking_ByObjects(&m_pPickedObj))       // ��ŷ�ߴٸ� PickedObj�� ���� �����̰�.
        {    
            m_pFreeCamera->Set_PickedObj(m_pPickedObj);                                     
        }
    }

    if (m_pGameInstance->Get_KeyState(KEY::CTRL) == KEY_STATE::TAP)
    {
        if (nullptr == m_pPickedObj)
        {
            MSG_BOX(TEXT("������ ������Ʈ�� ���� ��ŷ�Ͻʼ�"));
        }
        else
        {
            if (IsPicking_ByObjects(&m_pPickedObj))       // ��ŷ�ߴٸ� PickedObj�� ���� �����̰�.
            {
                m_pGameInstance->Delete(LEVEL_MAPTOOL, CRenderer::RENDERGROUP::RG_NONBLEND, m_pPickedObj);
                m_pPickedObj = nullptr;
            }
        }
       
    }


    if (m_pGameInstance->Get_KeyState(KEY::O) == KEY_STATE::TAP)
    {
        m_pFreeCamera->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 20.f, -10.f,1.f));
        m_pFreeCamera->Get_Transform()->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));
        m_pFreeCamera->Set_PickedObj(nullptr);
    }

  
    if (m_pGameInstance->Get_KeyState(KEY::P) == KEY_STATE::TAP)
    {
        m_pPickedObj = m_pGameInstance->Find_Player();
        m_pFreeCamera->Set_PickedObj(m_pPickedObj);
    }

    if (m_pGameInstance->Get_KeyState(KEY::U) == KEY_STATE::TAP)
    {
        CGameObject* pOwner = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Anim_Object").back();
        m_pPickedObj = static_cast<CPistol*>(pOwner)->Get_Part(CPistol::PARTID::PART_WEAPON);

        //m_pFreeCamera->Set_PickedObj(m_pPickedObj);
    }

    if (m_pGameInstance->Get_KeyState(KEY::Y) == KEY_STATE::TAP)
    {
        CGameObject* pOwner = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Anim_Object").back();
        m_pPickedObj = static_cast<CSniper*>(pOwner)->Get_Part(CSniper::PARTID::PART_WEAPON);

        //m_pFreeCamera->Set_PickedObj(m_pPickedObj);
    }


    if (m_pGameInstance->Get_KeyState(KEY::T) == KEY_STATE::TAP)
    {              
        // ī�޶� �÷��̾ ����ٳ���Ѵ�.
        CTransform* pCameraTransform = m_pFreeCamera->Get_Transform();
        CTransform* pPlayerTransform = m_pGameInstance->Find_Player()->Get_Transform();

        pCameraTransform->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
        pCameraTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
        pCameraTransform->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));

        //pPlayerTransform->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
        //pPlayerTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
        //pPlayerTransform->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));    
    }

}


HRESULT CLevel_MapTool::Render()
{
    SetWindowText(g_hWnd, TEXT("����_�����Դϴ�."));

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

 
    //ImGui_GridRender();

    
    ImGui_Render();
    Second_ImGui_Render();


    if (m_pPickedObj)
    {
        CContainerObject* pPickedObj = dynamic_cast<CContainerObject*>(m_pPickedObj);
        
        if (pPickedObj)
        {
            CModel* pModel = pPickedObj->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
            if (pModel && CModel::MODEL_TYPE::TYPE_ANIM == pModel->Get_ModelType())
            {
                Third_ImGui_Render();
            }
        }    
    }
    
    

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


    return S_OK;
}

HRESULT CLevel_MapTool::ImGui_GridRender()
{
   ImGuizmo::SetOrthographic(!m_bPerspectiveMode);
   ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY); // ����Ʈ ũ�� ����
   ImGuizmo::BeginFrame();

    // �׸��� ������
    static _float Worldmatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // ķ��忡���� �ٸ���
    static _float4x4 ViewMatrix = {};
    static _float4x4 ProjMatrix = {};

    if (m_bPerspectiveMode)
    {
        XMStoreFloat4x4(&ViewMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
        XMStoreFloat4x4(&ProjMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    }
    else
    {
        XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
        XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.1f, 1.f));
    }

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))      // ImGui â�� ������ ���� ��������
    {
        g_bFocusImGui = true;
    }
    else
        g_bFocusImGui = false;
    
   
    
    ImGuizmo::DrawGrid((_float*)(&ViewMatrix), (_float*)(&ProjMatrix), (_float*)(&Worldmatrix), 100.0f);
    
   
    return S_OK;
}


void CLevel_MapTool::ImGui_Render()
{  
    ImGui::SetNextWindowSize(ImVec2(400, 700));
    ImGui::Begin("First_ImGui");

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))      // ImGui â�� ������ ���� ��������
    {
        g_bFocusImGui = true;
    }
    else
        g_bFocusImGui = false;

    ImGuizmo::SetOrthographic(!m_bPerspectiveMode);
    ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY); // ����Ʈ ũ�� ����
    ImGuizmo::BeginFrame();
    
    if (ImGui::RadioButton("Perspective", m_bPerspectiveMode)) 
        m_bPerspectiveMode = true;
    ImGui::SameLine();
    if (ImGui::RadioButton("Orthographic", !m_bPerspectiveMode)) 
        m_bPerspectiveMode = false;
    
    
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"Space: �� ��ġ");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"Alt + LButton: �� ��ŷ");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"Ctrl + LButton: �� ����");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"O: ī�޶� �ʱ���ġ��");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"P: Player ��ŷ");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"T: Player Cam�� ����");
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    

    ImGui::TextColored(ImVec4(0, 1, 1, 1), u8"Picking Object Info");
 
    _bool bPicking = false;
    if (ImGui::Checkbox("Unlock Pick Object", &bPicking))
    {
        if (m_pPickedObj)
        {
            m_pPickedObj = nullptr;
        }
    }


    if (ImGui::Checkbox(u8"Can't Pick City: (City & Building ��������)", &g_PickingCity))
    {      
      
    }
    if (ImGui::Checkbox(u8"Can't Pick Terrain: (Terrain ��������)", &g_PickingTerrain))
    {

    }
    if (ImGui::Checkbox(u8"Player ����ٴϱ�", &g_FollowPlayer))
    {

    }   
    if (ImGui::Checkbox(u8"Player Animation Control", &g_PlayerAnimationControler))
    {

    }
    

    
    string PrototypeName = "PrototypeName >> ";
    string LayerName = "LayerName >> ";
    string Model_PrototypeName = "Model_PrototypeName >> ";

    if (m_pPickedObj)
    {        
        PrototypeName = wstringToString(m_pPickedObj->Get_PrototypeName()).c_str();
        LayerName = wstringToString(m_pPickedObj->Get_LayerName()).c_str();

        if (nullptr != m_pPickedObj->Get_Model())
            Model_PrototypeName = wstringToString(m_pPickedObj->Get_Model()->Get_PrototypeName()).c_str();
        else
            Model_PrototypeName = "No Model";
        

        ImGui::TextColored(ImVec4(0, 1, 0, 1), PrototypeName.c_str());
        ImGui::TextColored(ImVec4(0, 1, 0, 1), LayerName.c_str());
        ImGui::TextColored(ImVec4(0, 1, 0, 1), Model_PrototypeName.c_str());


        _vector vPosition = m_pPickedObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);

        string strX = "X: " + to_string(XMVectorGetX(vPosition));
        string strY = "Y: " + to_string(XMVectorGetY(vPosition));
        string strZ = "Z: " + to_string(XMVectorGetZ(vPosition));

        ImGui::TextColored(ImVec4(1, 1, 0, 1), strX.c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), strY.c_str());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), strZ.c_str());

    }
    else
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "No Picking Object");
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "No Picking Object");
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "No Picking Object");
    }
    
 

    if (ImGui::Button(("Terrian")))
    {      
        m_eMainCheckType = MAIN_CHECK_TYPE::TERRAIN;
    }
    ImGui::SameLine();
    if (ImGui::Button(("Object")))
    {
        m_eMainCheckType = MAIN_CHECK_TYPE::OBJECT;
    }
    ImGui::SameLine();
    if (ImGui::Button(("Camera")))
    {
        m_eMainCheckType = MAIN_CHECK_TYPE::CAMERA;
    }
    ImGui::SameLine();
    if (ImGui::Button(("Navigation")))
    {
        m_eMainCheckType = MAIN_CHECK_TYPE::NAVIGATION;
    }


    if (MAIN_CHECK_TYPE::NAVIGATION != m_eMainCheckType)
    {
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        if (ImGui::RadioButton("GuizmoMode: Translate", eGizmoType == ImGuizmo::TRANSLATE))
            eGizmoType = ImGuizmo::TRANSLATE;
        if (ImGui::RadioButton("GuizmoMode: Rotate", eGizmoType == ImGuizmo::ROTATE))
            eGizmoType = ImGuizmo::ROTATE;
        if (ImGui::RadioButton("GuizmoMode: Scale", eGizmoType == ImGuizmo::SCALE))
            eGizmoType = ImGuizmo::SCALE;


        ImGui::Dummy(ImVec2(0.0f, 15.0f)); // ���̰� 10�� �� ���� �߰� 
        ImGui::TextColored(ImVec4(1, 0, 0, 0), u8"�� Ȯ���ϰ� ����");

        if(ImGui::RadioButton(u8"Object SetType: Decorative", eObjectType == OBJECT_TYPE::DECORATIVE_OBJECT))
            eObjectType = OBJECT_TYPE::DECORATIVE_OBJECT;
        if (ImGui::RadioButton(u8"Object SetType: Static", eObjectType == OBJECT_TYPE::STATIC_OBJECT))
            eObjectType = OBJECT_TYPE::STATIC_OBJECT;
        if (ImGui::RadioButton(u8"Object SetType: Dynamic", eObjectType == OBJECT_TYPE::DYNAMIC_OBJECT))
            eObjectType = OBJECT_TYPE::DYNAMIC_OBJECT;
        
    }
    
   


  
    switch (m_eMainCheckType)
    {
    case Client::CLevel_MapTool::TERRAIN:
        Open_MapDialog();
        break;
    case Client::CLevel_MapTool::OBJECT:
        Open_ObjectDialog();
        break;
    case Client::CLevel_MapTool::CAMERA:
        Open_CameraDialog();
        break;
    case Client::CLevel_MapTool::NAVIGATION:
        Open_NavigationDialog();
        break;
    case Client::CLevel_MapTool::OTHER_MESH:
        Open_OtherModel();
        break;
    case Client::CLevel_MapTool::MAIN_CHECK_TYPE_END:
        break;
    default:
        break;
    }

  
    ImGui::End();
    



    //   =============== ImGuizmo =============== 

    if (nullptr != m_pPickedObj && nullptr == m_pFreeCamera->Get_PickedObj())  // ī�޶� ���� ���� ������Ʈ ��ġ�� ���������� nullptr�� ��ȯ
    {     
        static CGameObject* pPrePickedObj = nullptr;
        static _bool     bFristSetting = false;
        static _float4x4 WorldMatrix = {};
        static _float4x4 ViewMatrix = {};
        static _float4x4 ProjMatrix = {};


        if (!bFristSetting || pPrePickedObj != m_pPickedObj)
        {
            XMStoreFloat4x4(&WorldMatrix, m_pPickedObj->Get_Transform()->Get_WorldMatrix());

            pPrePickedObj = m_pPickedObj;
            bFristSetting = true;
        }               
        
        XMStoreFloat4x4(&ViewMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
        XMStoreFloat4x4(&ProjMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

        if (ImGuizmo::Manipulate((_float*)(&ViewMatrix), (_float*)(&ProjMatrix), eGizmoType, ImGuizmo::LOCAL, (_float*)(&WorldMatrix)))
        {
            if(false == g_NavigationOn)
                m_pPickedObj->Get_Transform()->Set_WorldMatrix(WorldMatrix);
        }        
    }

}

void CLevel_MapTool::Second_ImGui_Render()
{
    ImGui::SetNextWindowSize(ImVec2(300, 300));
    ImGui::Begin("Second_ImGui");


    string strNumTerrain = u8"Terrain Num: ";

    string strDecorativeObject = u8"Decorative_Object Num: ";
    string strStaticObject = u8"Static_Object Num: ";
    string strDynamicObject = u8"Dynamic_Object Num: ";


    string strNumAnimObject = u8"(�÷��̾� ����)AnimObject Num: ";
    


    list<CGameObject*>& TerrainList = m_pGameInstance->Get_GameObjects(LEVELID::LEVEL_MAPTOOL, L"Layer_BackGround_Terrain");


    list<CGameObject*>& DecorativeObjectList = m_pGameInstance->Get_GameObjects(LEVELID::LEVEL_MAPTOOL, L"Layer_Decorative_Object");
    list<CGameObject*>& StaticObjectList = m_pGameInstance->Get_GameObjects(LEVELID::LEVEL_MAPTOOL, L"Layer_Static_Object");
    list<CGameObject*>& DynamicObjectList = m_pGameInstance->Get_GameObjects(LEVELID::LEVEL_MAPTOOL, L"Layer_Dynamic_Object");

   

    list<CGameObject*>& AnimObjectList = m_pGameInstance->Get_GameObjects(LEVELID::LEVEL_MAPTOOL, L"Layer_Anim_Object");


    _uint iNumTerrain = TerrainList.size();

    _uint iNumDecorativeObject = DecorativeObjectList.size();
    _uint iNumStaticObject = StaticObjectList.size();
    _uint iNumDynamicObject = DynamicObjectList.size();

    _uint iNumAnimObject = AnimObjectList.size() + 1;   // << 1�� �÷��̾ �ǹ� 


    strNumTerrain += to_string(iNumTerrain);

   



     
    strDecorativeObject += to_string(iNumDecorativeObject);
    strStaticObject += to_string(iNumStaticObject);
    strDynamicObject += to_string(iNumDynamicObject);

    strNumAnimObject += to_string(iNumAnimObject);

 
   

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "CurLevel: LEVEL_MAPTOOL");
    ImGui::TextColored(ImVec4(1, 1, 1, 1), strNumTerrain.c_str());
    ImGui::TextColored(ImVec4(1, 1, 1, 1), strDecorativeObject.c_str());       // �� �ϴ� �� 1�� �ٸ� ���ҽ� ã�ƺ��� �����Ҽ������� �ϵ�������
    ImGui::TextColored(ImVec4(1, 1, 1, 1), strStaticObject.c_str());
    ImGui::TextColored(ImVec4(1, 1, 1, 1), strDynamicObject.c_str());

    ImGui::Dummy(ImVec2(0.0f, 20.0f)); // ���̰� 10�� �� ���� �߰�
    ImGui::TextColored(ImVec4(0, 1, 0, 1), strNumAnimObject.c_str());


    ImGui::End();
}



void CLevel_MapTool::Third_ImGui_Render()
{
    ImGui::SetNextWindowSize(ImVec2(600, 150));
    ImGui::Begin("Third_ImGui");


    CModel* pModel = static_cast<CContainerObject*>(m_pPickedObj)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

    CAnimation* pAnimation = pModel->Get_CurAnimation();


    _double Duration = pAnimation->Get_Duration();

    _double& CurTrackPosition = pModel->Get_Referene_CurrentTrackPosition();
    _float fPosition = static_cast<_float>(CurTrackPosition);


    _double SpeedPerSec = pAnimation->Get_SpeedPerSec();
    
    


    string  AnimationName = u8"���� �ִϸ��̼� �̸�: ";
    string  AnimationSpeedPerSec = u8"���� �ִϸ��̼� �ʴ�ӵ�: ";
    string  CurrentAnimIndex = u8"CurAnim / AllAnim: ";

    AnimationName += pAnimation->Get_Name();
    AnimationSpeedPerSec += to_string(SpeedPerSec);

    CurrentAnimIndex += to_string(pModel->Get_CurAnimationIndex());
    CurrentAnimIndex += " / ";
    CurrentAnimIndex += to_string(pModel->Get_NumAnimations());



    ImGui::SliderFloat(u8"���� �ִϸ��̼� ��ġ", (_float*)(&fPosition), 0.f, (_float)Duration);


    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // ���̰� 10�� �� ���� �߰�
    ImGui::TextColored(ImVec4(0, 1, 0, 1), AnimationName.c_str());             // �ִϸ��̼��� �̸� 
    ImGui::TextColored(ImVec4(0, 1, 0, 1), AnimationSpeedPerSec.c_str());      // �ִϸ��̼��� �ʴ� �ӵ�
    ImGui::TextColored(ImVec4(0, 1, 0, 1), CurrentAnimIndex.c_str());          // �ִϸ��̼��� ��ü �ε���

    

    ImGui::End();
}




void CLevel_MapTool::Open_MapDialog()
{
    if (nullptr != m_tPreViewObject.pTargetObject && !m_tPreViewObject.pTargetObject->Get_Dead())
    {
        m_pGameInstance->Delete(LEVEL_MAPTOOL, CRenderer::RENDERGROUP::RG_BLEND, m_tPreViewObject.pTargetObject); 
        m_tPreViewObject.pTargetObject = nullptr;
        m_tPreViewObject.pAdditionInfo = nullptr;

        m_bIsCreatePrewViewObject = false;
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

    ImGui::InputText(u8"Vertices_X InputCount:", m_szVerticesX, IM_ARRAYSIZE(m_szVerticesX));
    ImGui::InputText(u8"Vertices_Z InputCount:", m_szVerticesZ, IM_ARRAYSIZE(m_szVerticesZ));

  
    if (ImGui::Button(u8"Create Terrain"))
    {      
        _uint iX = atoi(m_szVerticesX);
        _uint iZ = atoi(m_szVerticesZ);
        if (iX <= 1 || iZ <= 1)
        {
            MSG_BOX(L"����,���ΰ� 1���̻��̿�����");
            return;
        }         
        else
        {
            if (nullptr != m_pPickedObj && m_pPickedObj->Get_LayerName() == L"Layer_BackGround_Terrain")
                m_pPickedObj = nullptr;

            /* For. Prototype_Component_VIBuffer_Terrain */
            
            _wstring TerrainBuffer_Prototype = L"Prototype_Component_VIBuffer_Terrain_";
            TerrainBuffer_Prototype += (to_wstring(iX) + L"_" + to_wstring(iZ));

            if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TerrainBuffer_Prototype,
                CVIBuffer_Terrain::Create(m_pDevice,m_pContext ,iX, iZ, nullptr))))
                assert(nullptr);


            _wstring Terrain_Prototype = L"Prototype_GameObject_Terrain_";
            Terrain_Prototype += (to_wstring(iX) + L"_" + to_wstring(iZ));

            if (FAILED(m_pGameInstance->Add_Prototype(Terrain_Prototype,
                CTerrain::Create(m_pDevice, m_pContext, iX, iZ))))
                assert(nullptr);
        
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, TEXT("Layer_BackGround_Terrain"), Terrain_Prototype)))
                assert(nullptr);
        }

    }

    ImGui::SameLine();

    if (ImGui::Button(u8"Modify Terrain"))
    {
        _uint iX = atoi(m_szVerticesX);
        _uint iZ = atoi(m_szVerticesZ);
        if (iX <= 1 || iZ <= 1)
        {
            MSG_BOX(L"����,���ΰ� 1���̻��̿�����");
            return;
        }
        else if (nullptr == m_pPickedObj || m_pPickedObj->Get_LayerName() != L"Layer_BackGround_Terrain")
        {
            MSG_BOX(L"������ ������ ���� ��ŷ�ϼ���.");
            return;
        }
        else
        {         
            /* For. Prototype_Component_VIBuffer_Terrain */

            _wstring TerrainBuffer_Prototype = L"Prototype_Component_VIBuffer_Terrain_";
            TerrainBuffer_Prototype += (to_wstring(iX) + L"_" + to_wstring(iZ));

            if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TerrainBuffer_Prototype,
                CVIBuffer_Terrain::Create(m_pDevice, m_pContext, iX, iZ, nullptr))))
                assert(nullptr);


            _wstring Terrain_Prototype = L"Prototype_GameObject_Terrain_";
            Terrain_Prototype += (to_wstring(iX) + L"_" + to_wstring(iZ));

            CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pPickedObj);

            pTerrain->Change_VIBuffer_Terrain(TerrainBuffer_Prototype);

            pTerrain->Set_PrototypeName(Terrain_Prototype);
        }

    }




   


    ImGui::Dummy(ImVec2(0.0f, 220.0f)); // ���̰� 10�� �� ���� �߰�

    if (ImGui::Button(u8"WIREFRAME ���κ���"))
    {
        //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    }
    ImGui::SameLine();
    if (ImGui::Button(u8"TEXTURE �κ���"))
    {
        //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }


    if (ImGui::Button(u8"Save"))
    {
        Save_Map();
    }        
    ImGui::SameLine();

    if (ImGui::Button(u8"Load"))
    {
        Load_Map();
    }
       
}

void CLevel_MapTool::Open_ObjectDialog()
{
    static _bool    bFirstCall = false;
    if (!bFirstCall)
    {
        if (FAILED(Ready_Object()))
        {
            MSG_BOX(TEXT("Failed Ready Obejcts"));
            assert(nullptr);
        }
        bFirstCall = true;
    }


    if (ImGui::Button(u8"���� Map �ҷ�����"))
    {
        _bool FinalMap = true;
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, L"Layer_Map", L"Prototype_GameObject_Static_Object", &FinalMap)))
            assert(nullptr);
    }





    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

    _int iSelectObjectNum = -1;     // ���� �������� -1 


    if (ImGui::TreeNode("Kit"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::KIT;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 5; ++i)
        {
            string label = "Kit" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }


        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }

    else if (ImGui::TreeNode("Debris"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::DEBRIS;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 2; ++i)
        {
            string label = "Debris" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }


        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }


    else if (ImGui::TreeNode("Barricade"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::BARRICADE;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 3; ++i)
        {
            string label = "Barricade" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }


    else if (ImGui::TreeNode("Billboard"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::BILLBOARD;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 12; ++i)
        {
            string label = "Billboard" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }


    else if (ImGui::TreeNode("Boss_Fight1"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::BOSS_FIGHT1;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 21; ++i)
        {
            string label = "Boss_Fight1_" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }


    else if (ImGui::TreeNode("Boss_Fight3"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::BOSS_FIGHT3;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 2; ++i)
        {
            string label = "Boss_Fight3_" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }


    else if (ImGui::TreeNode("CatWalk"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::CATWALK;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 6; ++i)
        {
            string label = "CatWalk" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }


    else if (ImGui::TreeNode("Crane"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::CRANE;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 3; ++i)
        {
            string label = "Crane" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }


    else if (ImGui::TreeNode("CyberCity"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::CYBERCITY;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 4; ++i)
        {
            string label = "CyberCity" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }



    else if (ImGui::TreeNode("Deco"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::DECO;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 6; ++i)
        {
            string label = "Deco" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }




    else if (ImGui::TreeNode("Fan"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::FAN;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 1; ++i)
        {
            string label = "Fan" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }



    else if (ImGui::TreeNode("Industrial_Elevator"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::INDUSTRIAL_ELEVATOR;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 1; ++i)
        {
            string label = "Industrial_Elevator" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }




    else if (ImGui::TreeNode("Lamp"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::LAMP;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 4; ++i)
        {
            string label = "Lamp" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }



    else if (ImGui::TreeNode("Pipe"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::PIPE;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 7; ++i)
        {
            string label = "Pipe" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }




    else if (ImGui::TreeNode("Platform"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::PLATFORM;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 4; ++i)
        {
            string label = "Platform" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }



    else if (ImGui::TreeNode("RotationFan"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::ROTATIONFAN;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 1; ++i)
        {
            string label = "RotationFan" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }



    else if (ImGui::TreeNode("Train"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::TRAIN;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 3; ++i)
        {
            string label = "Train" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }

    else if (ImGui::TreeNode("BodyBag"))
    {
    m_eModelCheckType = MODEL_CHECK_LIST::BODYBAG;
    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

    static _uint   iSelectKit = 0;
    for (size_t i = 0; i < 2; ++i)
    {
        string label = "BodyBag" + std::to_string(i);

        if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
        {
            iSelectKit = i;
        }
    }

    iSelectObjectNum = iSelectKit;
    ImGui::TreePop();
    }


    else if (ImGui::TreeNode("ClimbObject"))
    {
    m_eModelCheckType = MODEL_CHECK_LIST::CLIMBOBJECT;
    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

    static _uint   iSelectKit = 0;
    for (size_t i = 0; i < 1; ++i)
    {
        string label = "ClimbObject" + std::to_string(i);

        if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
        {
            iSelectKit = i;
        }
    }

    iSelectObjectNum = iSelectKit;
    ImGui::TreePop();
    }


    else if (ImGui::TreeNode("Sign"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::SIGN;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 6; ++i)
        {
            string label = "Sign" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }


        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }

    else if (ImGui::TreeNode("Wall"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::WALL;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 3; ++i)
        {
            string label = "Wall" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }


        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }

    else if (ImGui::TreeNode("Russian_Sign"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::RUSSIAN_SIGN;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 11; ++i)
        {
            string label = "Russian_Sign" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }


        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }



    else if (ImGui::TreeNode("City"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::CITY;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 1; ++i)
        {
            string label = "City" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }



    else if (ImGui::TreeNode("Building"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::BUILDING;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 5; ++i)
        {
            string label = "Building" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }


    else if (ImGui::TreeNode("Wire"))
    {
        m_eModelCheckType = MODEL_CHECK_LIST::WIRE;
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;
        for (size_t i = 0; i < 3; ++i)
        {
            string label = "Wire" + std::to_string(i);

            if (ImGui::RadioButton(label.c_str(), iSelectKit == i))
            {
                iSelectKit = i;
            }
        }

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }


   

    else if (ImGui::TreeNode("Monster"))
    {
       
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

        static _uint   iSelectKit = 0;


        //m_eModelCheckType = MODEL_CHECK_LIST::SPIDER;

        string label = "Spider";
        if (ImGui::RadioButton(label.c_str(), iSelectKit == 0))
        {
            m_eModelCheckType = MODEL_CHECK_LIST::SPIDER;
            iSelectKit = 0;
        }

        label = "Elite";
        if (ImGui::RadioButton(label.c_str(), iSelectKit == 1))
        {
            m_eModelCheckType = MODEL_CHECK_LIST::ELITE;
            iSelectKit = 1;
        }

        label = "Jetpack";
        if (ImGui::RadioButton(label.c_str(), iSelectKit == 2))
        {
            m_eModelCheckType = MODEL_CHECK_LIST::JETPACK;
            iSelectKit = 2;
        }

        label = "Mira";
        if (ImGui::RadioButton(label.c_str(), iSelectKit == 3))
        {
            m_eModelCheckType = MODEL_CHECK_LIST::MIRA;
            iSelectKit = 3;
        }

        label = "Pistol";
        if (ImGui::RadioButton(label.c_str(), iSelectKit == 4))
        {
            m_eModelCheckType = MODEL_CHECK_LIST::PISTOL;
            iSelectKit = 4;
        }

        label = "Sniper";
        if (ImGui::RadioButton(label.c_str(), iSelectKit == 5))
        {
            m_eModelCheckType = MODEL_CHECK_LIST::SNIPER;
            iSelectKit = 5;
        }

        label = "Hel";
        if (ImGui::RadioButton(label.c_str(), iSelectKit == 6))
        {
            m_eModelCheckType = MODEL_CHECK_LIST::HEL;
            iSelectKit = 6;
        }
        
      

        iSelectObjectNum = iSelectKit;
        ImGui::TreePop();
    }




    Create_PreViewObject(iSelectObjectNum);
}


void CLevel_MapTool::Open_CameraDialog()
{
    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�

    CTransform* pTransform = m_pFreeCamera->Get_Transform();
    _float fOriginCamSpeed = pTransform->Get_OriginSpeedPerSec();

    static _float fRatio = {1.f};
    ImGui::SliderFloat("Speed Ratio", &fRatio, 1.f, 40.f);


    

    pTransform->Set_SpeedPerSec(fOriginCamSpeed * fRatio);
}


void CLevel_MapTool::Open_NavigationDialog()
{
    m_MousePos = m_pGameInstance->Get_PickingBackGround_JustPos(m_fMouseY);

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�


    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"G: �׺��� On/Off");
    if (m_pGameInstance->Get_KeyState(KEY::G) == KEY_STATE::TAP)
    {
        if (true == g_NavigationOn)
            g_NavigationOn = false;
        else
            g_NavigationOn = true;
        m_pPickedObj->Get_Navigation()->Set_Modifiable(g_NavigationOn);
    }
       

    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"V: �̿� �����ϱ�");
    if (m_pGameInstance->Get_KeyState(KEY::V) == KEY_STATE::TAP)
    {
        if (nullptr == m_pPickedObj)
        {
            MSG_BOX(L"��ŷ������Ʈ�� null�Դϴ�");
            return;
        }

        CNavigation* pPickedObjNavigation = m_pPickedObj->Get_Navigation();
        CNavigation* pCameraNavigation = m_pFreeCamera->Get_Navigation();

        if (FAILED(pPickedObjNavigation->SetUp_Neighbors()))
            MSG_BOX(L"PickedObj �̿����� ����");

        if (FAILED(pCameraNavigation->SetUp_Neighbors()))
            MSG_BOX(L"Cam �̿����� ����");

        MSG_BOX(L"�̿����� �Ϸ�");

        g_NavigationOn = false;
    }

    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"B: �׺񿵿� �����ϱ�");
    if (m_pGameInstance->Get_KeyState(KEY::B) == KEY_STATE::TAP)
    {
        if (nullptr == m_pPickedObj)
        {
            MSG_BOX(L"������ ����� �׺���̼��� �ް��ִ�\n��Ȥ�� ������ ��ŷ�� �����ؾ��մϴ�.");
            return;
        }

        if (FAILED(m_pPickedObj->Get_Navigation()->GetUp_BoundingBox()))
            MSG_BOX(L"�׺� �ٿ���ڽ� ��������");
        else
            MSG_BOX(L"�׺� �ٿ���ڽ� �����Ϸ�");
    }
        

        


    if (ImGui::Checkbox("Navigation_Picking On", &g_NavigationOn))
    {
        if (nullptr == m_pPickedObj)
        {
            g_NavigationOn = false;
            MSG_BOX(L"������Ʈ�� ���� ��ŷ�ϼ���");
        }
        else if (nullptr == m_pPickedObj->Get_Navigation())
        {
            g_NavigationOn = false;
            MSG_BOX(L"�׺���̼��� ���� ������Ʈ �Դϴ�.");
        }
        else
            m_pPickedObj->Get_Navigation()->Set_Modifiable(g_NavigationOn);
     
    }
    
   
    ImGui::InputFloat(u8"Mouse Y����", &m_fMouseY);

    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"Ctrl+Z: ���� �� ����");

    if (g_NavigationOn)
    {
        vector<CCell*>& Cells = m_pPickedObj->Get_Navigation()->Get_Cells();

        string CellSize = "Cell Size: " + to_string(Cells.size());
        ImGui::TextColored(ImVec4(1, 1, 0, 1), CellSize.c_str());
    }
    

    string Cell_0_PosX = to_string(m_CellPoints[0].x);
    string Cell_0_PosY = to_string(m_CellPoints[0].y);
    string Cell_0_PosZ = to_string(m_CellPoints[0].z);

    string Cell_1_PosX = to_string(m_CellPoints[1].x);
    string Cell_1_PosY = to_string(m_CellPoints[1].y);
    string Cell_1_PosZ = to_string(m_CellPoints[1].z);

    string Cell_2_PosX = to_string(m_CellPoints[2].x);
    string Cell_2_PosY = to_string(m_CellPoints[2].y);
    string Cell_2_PosZ = to_string(m_CellPoints[2].z);

    string MousePosX = to_string(m_MousePos.x);     // 1031���ٿ��� ä����
    string MousePosY = to_string(m_MousePos.y);
    string MousePosZ = to_string(m_MousePos.z);

    string Cell_0_Info = "";
    string Cell_1_Info = "";
    string Cell_2_Info = "";
    string MousePosInfo = "";
    

    Cell_0_Info = "CellPoint[0]: X: " + Cell_0_PosX + " Y: " + Cell_0_PosY + " Z: " + Cell_0_PosZ;
    Cell_1_Info = "CellPoint[1]: X: " + Cell_1_PosX + " Y: " + Cell_1_PosY + " Z: " + Cell_1_PosZ;
    Cell_2_Info = "CellPoint[2]: X: " + Cell_2_PosX + " Y: " + Cell_2_PosY + " Z: " + Cell_2_PosZ;

    MousePosInfo = "MousePos: X: " + MousePosX + " Y: " + MousePosY + " Z: " + MousePosZ;

    ImGui::Text(Cell_0_Info.c_str());
    ImGui::Text(Cell_1_Info.c_str());
    ImGui::Text(Cell_2_Info.c_str());


    ImGui::Dummy(ImVec2(0.0f, 7.0f)); // ���̰� 10�� �� ���� �߰�
    ImGui::Text(MousePosInfo.c_str());


    if (ImGui::Button(u8"Save_Navigation"))
    {
        Save_Navigation();
    }
    ImGui::SameLine();
    if (ImGui::Button(u8"Load_Navigation"))
    {
        Load_Navigation();
    }
}


void CLevel_MapTool::Open_UIDialog()
{
    if (ImGui::Button(u8"UI TEST"))
    {

    }
}


void CLevel_MapTool::Save_OtherModel()
{
    FILE* fout = fopen("../Bin/Ohter_Model_Data.bin", "wb");
    if (!fout)    // ���� ���⿡ �����ߴٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }

    CModel* pModel = nullptr; 

    _uint iModelSize = 6;   // �� , �������� ,�Ѿ� 
    fwrite(&iModelSize, sizeof(iModelSize), 1, fout);

    for (_uint i = 1; i < iModelSize + 1; i++)
    {
        switch (i)
        {
        case 1:
            pModel = static_cast<CModel*>(m_pGameInstance->Find_Model(LEVEL_MAPTOOL, L"Prototype_Component_Blood1"));
            break;
        case 2:
            pModel = static_cast<CModel*>(m_pGameInstance->Find_Model(LEVEL_MAPTOOL, L"Prototype_Component_Piece"));
            break;
        case 3:
            pModel = static_cast<CModel*>(m_pGameInstance->Find_Model(LEVEL_MAPTOOL, L"Prototype_Component_Bullet"));
            break;
        case 4:
            pModel = static_cast<CModel*>(m_pGameInstance->Find_Model(LEVEL_MAPTOOL, L"Prototype_Component_Sky"));
            break;
        case 5:
            pModel = static_cast<CModel*>(m_pGameInstance->Find_Model(LEVEL_MAPTOOL, L"Prototype_Component_Electric"));
            break;
        case 6:
            pModel = static_cast<CModel*>(m_pGameInstance->Find_Model(LEVEL_MAPTOOL, L"Prototype_Component_BigElectric"));
            break;
        default:
            break;
        }


        std::string modelPrototypeName = wstringToString(pModel->Get_PrototypeName());
        std::string modelFilePath = wstringToString(pModel->Get_m_strModelFilePath());

        _uint modelPrototypeNameSize = modelPrototypeName.size();
        _uint modelFilePathSize = modelFilePath.size();

        _uint meshCount = pModel->Get_MeshesCount();
        _uint materialsCount = pModel->Get_MaterialsCount();

        _float4x4 PreTransformMatrix = pModel->Get_PreTransformMatrix();
        fwrite(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fout);

        fwrite(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fout);
        fwrite(modelPrototypeName.c_str(), modelPrototypeNameSize, 1, fout);


        fwrite(&modelFilePathSize, sizeof(modelFilePathSize), 1, fout); // fbx ���� ��� ũ��
        fwrite(modelFilePath.c_str(), modelFilePathSize, 1, fout);

        fwrite(&meshCount, sizeof(meshCount), 1, fout);
        fwrite(&materialsCount, sizeof(materialsCount), 1, fout);

        for (size_t i = 0; i < materialsCount; i++)
        {
            _wstring* pwstrMaterialTexturePath = pModel->Get_MaterialTexturePath(i);

            for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
            {
                std::string materialTexturePath = wstringToString(pwstrMaterialTexturePath[j]);
                _uint pathSize = materialTexturePath.size();
                if (pathSize == 0)
                {
                    std::string noPath = "NO_PATH";
                    _uint noPathSize = noPath.size();

                    fwrite(&noPathSize, sizeof(noPathSize), 1, fout);
                    fwrite(noPath.c_str(), noPathSize, 1, fout);
                }
                else
                {
                    fwrite(&pathSize, sizeof(pathSize), 1, fout);
                    fwrite(materialTexturePath.c_str(), pathSize, 1, fout);
                }
            }
        }

        vector<CMesh*>& vecMesh = pModel->Get_Meshes();
        for (size_t i = 0; i < vecMesh.size(); i++)
        {
            _uint materialIndex = vecMesh[i]->Get_MaterialIndex();
            _uint numVertices = vecMesh[i]->m_iNumVertices;
            _uint vertexStride = vecMesh[i]->m_iVertexStride;

            fwrite(&materialIndex, sizeof(materialIndex), 1, fout);
            fwrite(&numVertices, sizeof(numVertices), 1, fout);
            fwrite(&vertexStride, sizeof(vertexStride), 1, fout);

            VTXMESH* pVertices = vecMesh[i]->Get_Vertices();
            for (size_t j = 0; j < numVertices; j++)
            {
                fwrite(&pVertices[j].vPosition, sizeof(pVertices[j].vPosition), 1, fout);
                fwrite(&pVertices[j].vNormal, sizeof(pVertices[j].vNormal), 1, fout);
                fwrite(&pVertices[j].vTexcoord, sizeof(pVertices[j].vTexcoord), 1, fout);
                fwrite(&pVertices[j].vTangent, sizeof(pVertices[j].vTangent), 1, fout);
            }

            _uint numIndices = vecMesh[i]->m_iNumIndices;
            _uint indexStride = vecMesh[i]->m_iIndexStride;
            fwrite(&numIndices, sizeof(numIndices), 1, fout);
            fwrite(&indexStride, sizeof(indexStride), 1, fout);

            _uint* pIndices = vecMesh[i]->Get_Indices();
            fwrite(pIndices, sizeof(_uint), numIndices, fout);

            fwrite(&vecMesh[i]->m_eIndexFormat, sizeof(vecMesh[i]->m_eIndexFormat), 1, fout);
            fwrite(&vecMesh[i]->m_eTopology, sizeof(vecMesh[i]->m_eTopology), 1, fout);

            fwrite(&vecMesh[i]->Get_MinPos(), sizeof(vecMesh[i]->Get_MinPos()), 1, fout);
            fwrite(&vecMesh[i]->Get_MaxPos(), sizeof(vecMesh[i]->Get_MaxPos()), 1, fout);
        }
    }
    

    





    fclose(fout);
    MSG_BOX(TEXT("Other ������ ����!"));

}


void CLevel_MapTool::Save_FinalMap()
{
    FILE* fout = fopen("../Bin/FinalMap_Data.bin", "wb");
    if (!fout)    // ���� ���⿡ �����ߴٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }

    CModel* pModel = nullptr;

    _uint iModelSize = 1;
    fwrite(&iModelSize, sizeof(iModelSize), 1, fout);

    for (_uint i = 1; i < iModelSize + 1; i++)
    {
        switch (i)
        {
        case 1:
            pModel = static_cast<CModel*>(m_pGameInstance->Find_Model(LEVEL_MAPTOOL, L"Prototype_Component_MapModel"));
            break;
        default:
            break;
        }


        std::string modelPrototypeName = wstringToString(pModel->Get_PrototypeName());
        std::string modelFilePath = wstringToString(pModel->Get_m_strModelFilePath());

        _uint modelPrototypeNameSize = modelPrototypeName.size();
        _uint modelFilePathSize = modelFilePath.size();

        _uint meshCount = pModel->Get_MeshesCount();
        _uint materialsCount = pModel->Get_MaterialsCount();

        _float4x4 PreTransformMatrix = pModel->Get_PreTransformMatrix();
        fwrite(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fout);

        fwrite(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fout);
        fwrite(modelPrototypeName.c_str(), modelPrototypeNameSize, 1, fout);


        fwrite(&modelFilePathSize, sizeof(modelFilePathSize), 1, fout); // fbx ���� ��� ũ��
        fwrite(modelFilePath.c_str(), modelFilePathSize, 1, fout);

        fwrite(&meshCount, sizeof(meshCount), 1, fout);
        fwrite(&materialsCount, sizeof(materialsCount), 1, fout);

        for (size_t i = 0; i < materialsCount; i++)
        {
            _wstring* pwstrMaterialTexturePath = pModel->Get_MaterialTexturePath(i);

            for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
            {
                std::string materialTexturePath = wstringToString(pwstrMaterialTexturePath[j]);
                _uint pathSize = materialTexturePath.size();
                if (pathSize == 0)
                {
                    std::string noPath = "NO_PATH";
                    _uint noPathSize = noPath.size();

                    fwrite(&noPathSize, sizeof(noPathSize), 1, fout);
                    fwrite(noPath.c_str(), noPathSize, 1, fout);
                }
                else
                {
                    fwrite(&pathSize, sizeof(pathSize), 1, fout);
                    fwrite(materialTexturePath.c_str(), pathSize, 1, fout);
                }
            }
        }

        vector<CMesh*>& vecMesh = pModel->Get_Meshes();
        for (size_t i = 0; i < vecMesh.size(); i++)
        {
            _uint materialIndex = vecMesh[i]->Get_MaterialIndex();
            _uint numVertices = vecMesh[i]->m_iNumVertices;
            _uint vertexStride = vecMesh[i]->m_iVertexStride;

            fwrite(&materialIndex, sizeof(materialIndex), 1, fout);
            fwrite(&numVertices, sizeof(numVertices), 1, fout);
            fwrite(&vertexStride, sizeof(vertexStride), 1, fout);

            VTXMESH* pVertices = vecMesh[i]->Get_Vertices();
            for (size_t j = 0; j < numVertices; j++)
            {
                fwrite(&pVertices[j].vPosition, sizeof(pVertices[j].vPosition), 1, fout);
                fwrite(&pVertices[j].vNormal, sizeof(pVertices[j].vNormal), 1, fout);
                fwrite(&pVertices[j].vTexcoord, sizeof(pVertices[j].vTexcoord), 1, fout);
                fwrite(&pVertices[j].vTangent, sizeof(pVertices[j].vTangent), 1, fout);
            }

            _uint numIndices = vecMesh[i]->m_iNumIndices;
            _uint indexStride = vecMesh[i]->m_iIndexStride;
            fwrite(&numIndices, sizeof(numIndices), 1, fout);
            fwrite(&indexStride, sizeof(indexStride), 1, fout);

            _uint* pIndices = vecMesh[i]->Get_Indices();
            fwrite(pIndices, sizeof(_uint), numIndices, fout);

            fwrite(&vecMesh[i]->m_eIndexFormat, sizeof(vecMesh[i]->m_eIndexFormat), 1, fout);
            fwrite(&vecMesh[i]->m_eTopology, sizeof(vecMesh[i]->m_eTopology), 1, fout);

            fwrite(&vecMesh[i]->Get_MinPos(), sizeof(vecMesh[i]->Get_MinPos()), 1, fout);
            fwrite(&vecMesh[i]->Get_MaxPos(), sizeof(vecMesh[i]->Get_MaxPos()), 1, fout);
        }
    }








    fclose(fout);
    MSG_BOX(TEXT("FinalMap ������ ����!"));

}

void CLevel_MapTool::Load_Final_Map_MapTool()
{
    FILE* fin = fopen("../Bin/FinalMap.bin", "rb");
    if (!fin)    // ���� ���⿡ �����ߴٸ�
    {
        MSG_BOX(TEXT("���� �б⸦ �����߾��.."));
        return;
    }

    CModel* pModel = nullptr;

    _uint iModelSize = 0;
    fread(&iModelSize, sizeof(iModelSize), 1, fin);

    for (_uint i = 1; i < iModelSize + 1; i++)
    {
        // PreTransformMatrix �б�
        _float4x4 PreTransformMatrix;
        fread(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fin);

        // Prototype Name ũ�� �б�
        _uint modelPrototypeNameSize = 0;
        fread(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fin);

        // Prototype Name �б�
        std::string modelPrototypeName(modelPrototypeNameSize, '\0');
        fread(&modelPrototypeName[0], modelPrototypeNameSize, 1, fin);

        // Model File Path ũ�� �б�
        _uint modelFilePathSize = 0;
        fread(&modelFilePathSize, sizeof(modelFilePathSize), 1, fin);

        // Model File Path �б�
        std::string modelFilePath(modelFilePathSize, '\0');
        fread(&modelFilePath[0], modelFilePathSize, 1, fin);

        // Mesh �� Material ���� �б�
        _uint meshCount = 0;
        _uint materialsCount = 0;
        fread(&meshCount, sizeof(meshCount), 1, fin);
        fread(&materialsCount, sizeof(materialsCount), 1, fin);

        string					strMaterialTexturePath = "";
        _wstring				wstrMaterialTexturePath[200][AI_TEXTURE_TYPE_MAX] = { L"" };			// << �̰� �����ʵ����ʹ� ���׸��󰹼�����

        // Material Texture Path �б�
        for (size_t i = 0; i < materialsCount; i++)
        {
            for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
            {
                ReadString(fin, strMaterialTexturePath);
                wstrMaterialTexturePath[i][j] = stringToWstring(strMaterialTexturePath); // ���׸��� ���
            }
        }


        _wstring ModelPrototypeName = stringToWstring(modelPrototypeName);

        CMesh::MESH_DESC* pDesc = new CMesh::MESH_DESC[meshCount];
        pDesc->isInstanceObject = true;
        pDesc->InstanceBufferPrototypeTag = ModelPrototypeName + L"Instance";


        for (size_t i = 0; i < meshCount; i++)
        {
            fread(&pDesc[i].iMaterialIndex, sizeof(pDesc[i].iMaterialIndex), 1, fin);
            fread(&pDesc[i].iNumVertices, sizeof(pDesc[i].iNumVertices), 1, fin);
            fread(&pDesc[i].iVertexStride, sizeof(pDesc[i].iVertexStride), 1, fin);

            pDesc[i].pVertices = new VTXMESH[pDesc[i].iNumVertices];
            fread(pDesc[i].pVertices, sizeof(VTXMESH) * pDesc[i].iNumVertices, 1, fin);

            fread(&pDesc[i].iNumIndices, sizeof(pDesc[i].iNumIndices), 1, fin);
            fread(&pDesc[i].iIndexStride, sizeof(pDesc[i].iIndexStride), 1, fin);

            pDesc[i].pIndices = new _uint[pDesc[i].iNumIndices];
            fread(pDesc[i].pIndices, sizeof(_uint) * pDesc[i].iNumIndices, 1, fin);

            fread(&pDesc[i].eIndexFormat, sizeof(pDesc[i].eIndexFormat), 1, fin);
            fread(&pDesc[i].eTopology, sizeof(pDesc[i].eTopology), 1, fin);

            fread(&pDesc[i].vMinPos, sizeof(pDesc[i].vMinPos), 1, fin);
            fread(&pDesc[i].vMaxPos, sizeof(pDesc[i].vMaxPos), 1, fin);
        }

        if (true == m_pGameInstance->IsFind_Model(LEVEL_GAMEPLAY, ModelPrototypeName)) // ���� �ҷ������� �ϴ� ���� �̹� �ҷ��Ծ�?
        {
            // ���� ã���� ���� ó��
        }
        else
        {
            if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ModelPrototypeName, // �� ������Ÿ�� �̸�
                CModel::Create(m_pDevice, m_pContext, CModel::MODEL_TYPE::TYPE_NONANIM, // ��Ÿ��
                    modelFilePath.c_str(), // �� fbx ���
                    *wstrMaterialTexturePath, // ���׸��� �ؽ��� ���
                    meshCount, // �޽� ����
                    materialsCount, // ���׸��� ����
                    XMLoadFloat4x4(&PreTransformMatrix), // ����
                    pDesc)))) // �޽��� �̷�� ���� ����
            {
                _wstring msg = to_wstring(i) + L"��°" + L"���� �ε尡 �ȴ�";
                MSG_BOX(msg.c_str());
                return ;
            }
        }


        for (size_t i = 0; i < meshCount; i++)
        {
            Safe_Delete_Array(pDesc[i].pVertices);
            Safe_Delete_Array(pDesc[i].pIndices);
        }
        Safe_Delete_Array(pDesc);
    }

    fclose(fin);

    MSG_BOX(TEXT("���� �� ���� Load�߽��ϴ�.\n OpenObject�� �纻 ������ ������ ����ϴ�."));
}

void CLevel_MapTool::Open_OtherModel()
{
    if (ImGui::Button(u8"Other Mesh"))
    {

    }
}




HRESULT CLevel_MapTool::Create_PreViewObject(_int iSelectObjectNum)
{
    if (iSelectObjectNum == -1)
        return S_OK;

    
    static OBJECT_TYPE       ePreObjectCheckType = OBJECT_TYPE::OBJECT_TYPE_END;
    static MODEL_CHECK_LIST  ePreModelCheckType = MODEL_CHECK_LIST::MODEL_CHECK_TYPE_END;
    static _int              iPreObjectSelectNum = -1;

   
    if (m_bIsCreatePrewViewObject == true &&
        ePreModelCheckType == m_eModelCheckType &&
        iPreObjectSelectNum == iSelectObjectNum &&
        ePreObjectCheckType == eObjectType)
        return S_OK;
      



    _wstring strLayerName = L"Layer_PreView_Object";
    _wstring strPrototypeName = L"";
  

    // ��üũŸ�Կ� �����°� �ƴ϶� 
    // ���������� ��ü�� Ŭ���� Ÿ������ ������.



    switch (eObjectType)
    {
    case OBJECT_TYPE::DECORATIVE_OBJECT:
        strPrototypeName = TEXT("Prototype_GameObject_Decorative_Object");
        break;
    case OBJECT_TYPE::STATIC_OBJECT:
        strPrototypeName = TEXT("Prototype_GameObject_Static_Object");
        break;
    case OBJECT_TYPE::DYNAMIC_OBJECT:
        strPrototypeName = TEXT("Prototype_GameObject_Dynamic_Object");
        break;

    default:    // �ӽ÷� ������ ��
        break;
    }


    switch (m_eModelCheckType)      // ��ü������ ����Ÿ�� �̸�
    {
    case Client::CLevel_MapTool::SPIDER:
        strPrototypeName = TEXT("Prototype_GameObject_Spider");
        break;
    case Client::CLevel_MapTool::ELITE:
        strPrototypeName = TEXT("Prototype_GameObject_Elite");
        break;
    case Client::CLevel_MapTool::JETPACK:
        strPrototypeName = TEXT("Prototype_GameObject_Jetpack");
        break;

    case Client::CLevel_MapTool::MIRA:
        strPrototypeName = TEXT("Prototype_GameObject_Mira");
        break;
    case Client::CLevel_MapTool::PISTOL:
        strPrototypeName = TEXT("Prototype_GameObject_Pistol");
        break;
    case Client::CLevel_MapTool::SNIPER:
        strPrototypeName = TEXT("Prototype_GameObject_Sniper");
        break;

    case Client::CLevel_MapTool::HEL:
        strPrototypeName = TEXT("Prototype_GameObject_Hel");
        break;
    default:
        break;
    }




    static CGameObject::GAMEOBJECT_DESC Desc = {};

    Desc.fRotationPerSec = 20.f;
    Desc.fSpeedPerSec = 20.f;
    Desc.iModelNum = iSelectObjectNum;
    Desc.iModelListType = m_eModelCheckType;

    // ó�� ��������� �Ŵ�?
    if (!m_bIsCreatePrewViewObject)
    {
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, strLayerName, strPrototypeName, &Desc)))
            assert(nullptr);

        m_bIsCreatePrewViewObject = true;

        ePreObjectCheckType = eObjectType;
        ePreModelCheckType = m_eModelCheckType;
        iPreObjectSelectNum = iSelectObjectNum;
    }
    // ������ üũ�ߴ��Ŷ� ���� üũ�Ѱ� �ٸ��ų� Or ���� ������ ������Ʈ�� �𵨳ѹ��� �ٸ���?
    else if (ePreModelCheckType != m_eModelCheckType || iPreObjectSelectNum != iSelectObjectNum || ePreObjectCheckType != eObjectType)
    {
        //  �̸����� ������Ʈ�� �ٲ����ϴ� ����
        //  ���� ������ ������Ʈ�� Ÿ���� �޶�, Or ���� ������ ������Ʈ�� Ÿ���� ������ ���� ������ ���� �ٸ���� ex) ���� Kit������Ʈ����
        //  ���� ������ ���� �ٸ���쵵 �����Ѵ�.
      
        CGameObject* pPreViewObject = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_PreView_Object").back();

        m_pGameInstance->Delete(LEVEL_MAPTOOL , CRenderer::RENDERGROUP::RG_BLEND ,pPreViewObject);

        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, strLayerName, strPrototypeName, &Desc)))
            assert(nullptr);

        ePreObjectCheckType = eObjectType;
        ePreModelCheckType = m_eModelCheckType;
        iPreObjectSelectNum = iSelectObjectNum;
    }

    CGameObject* pPreViewObject = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_PreView_Object").back();

    m_tPreViewObject.pTargetObject = pPreViewObject;
    m_tPreViewObject.pAdditionInfo = &Desc;
   
    return S_OK;
}



HRESULT CLevel_MapTool::Create_Object()
{
    _wstring strLayerName = L"";
    _wstring strPrototypeName = m_tPreViewObject.pTargetObject->Get_PrototypeName();


    switch (eObjectType)
    {
    case Client::CLevel_MapTool::DECORATIVE_OBJECT:
        strLayerName = L"Layer_Decorative_Object";
        break;
    case Client::CLevel_MapTool::STATIC_OBJECT:
        strLayerName = L"Layer_Static_Object";
        break;
    case Client::CLevel_MapTool::DYNAMIC_OBJECT:
        strLayerName = L"Layer_Dynamic_Object";
        break;
    case Client::CLevel_MapTool::OBJECT_TYPE_END:
        break;
    default:
        break;
    }



    switch (m_eModelCheckType)
    {
    case Client::CLevel_MapTool::SPIDER:
        strLayerName = L"Layer_Anim_Object";
        break;
    case Client::CLevel_MapTool::ELITE:
        strLayerName = L"Layer_Anim_Object";
        break;
    case Client::CLevel_MapTool::JETPACK:
        strLayerName = L"Layer_Anim_Object";
        break;

    case Client::CLevel_MapTool::MIRA:
        strLayerName = L"Layer_Anim_Object";
        break;
    case Client::CLevel_MapTool::PISTOL:
        strLayerName = L"Layer_Anim_Object";
        break;
    case Client::CLevel_MapTool::SNIPER:
        strLayerName = L"Layer_Anim_Object";
        break;
    case Client::CLevel_MapTool::HEL:
        strLayerName = L"Layer_Anim_Object";
        break;
    default:
        break;
    }


  




    CGameObject::GAMEOBJECT_DESC* Desc = nullptr;
    Desc = static_cast<CGameObject::GAMEOBJECT_DESC*>(m_tPreViewObject.pAdditionInfo);
    Desc->InitWorldMatrix = m_tPreViewObject.pTargetObject->Get_Transform()->Get_WorldMatrix();
    Desc->iModelListType = m_eModelCheckType; // ŰƮ ,���� , ���
    Desc->iObjectType = eObjectType;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, strLayerName, strPrototypeName, Desc)))
        assert(nullptr);


    return S_OK;
}



_bool CLevel_MapTool::IsPicking_ByObjects(CGameObject** pOut)
{
    _float3     vPickPos = {};


    list<CGameObject*>& AnimObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Anim_Object");
    for (auto pAnimObj : AnimObjects)
    {
        CModel* pModel = dynamic_cast<CModel*>(pAnimObj->Find_Component(L"Com_Model"));
        vector<CMesh*>& vecMeshs = pModel->Get_Meshes();

        for (size_t i = 0; i < vecMeshs.size(); i++)
        {
            _float3 vPickPos;                       // ������� ��ġ�� ��ȯ�ǰ�,
            
            if (vecMeshs[i]->isPicking(pAnimObj->Get_Transform()->Get_WorldMatrix(), &vPickPos))
            {
                // �ش� ���� �޽��� �ϳ��� ��ŷ�� �ߴٸ�

                *pOut = pAnimObj;
                return true;            // ������Ʈ �켱���� ��ŷ�ߴ�? ����� ��ŷ�������� �ͷ��� ����� 
            }
        }
    }


    list<CGameObject*>& DynamicObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Dynamic_Object");
    for (auto pGameObj : DynamicObjects)
    {
        if (true == g_PickingCity)
        {
            CLevel_MapTool::MODEL_CHECK_LIST eType = (CLevel_MapTool::MODEL_CHECK_LIST)pGameObj->Get_ModelListType();

            if (CLevel_MapTool::MODEL_CHECK_LIST::CITY == eType ||
                CLevel_MapTool::MODEL_CHECK_LIST::BUILDING == eType)
            {
                continue;
            }
        }

        CModel* pModel = dynamic_cast<CModel*>(pGameObj->Find_Component(L"Com_Model"));
        vector<CMesh*>& vecMeshs = pModel->Get_Meshes();

        for (size_t i = 0; i < vecMeshs.size(); i++)
        {
            if (vecMeshs[i]->isPicking(pGameObj->Get_Transform()->Get_WorldMatrix(), &vPickPos))
            {
                // �ش� ���� �޽��� �ϳ��� ��ŷ�� �ߴٸ�

                *pOut = pGameObj;
                return true;            // ������Ʈ �켱���� ��ŷ�ߴ�? ����� ��ŷ�������� �ͷ��� ����� 
            }
        }
    }




    list<CGameObject*>& DecorativeObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Decorative_Object");
    for (auto pGameObj : DecorativeObjects)
    {
        if (true == g_PickingCity)
        {
            CLevel_MapTool::MODEL_CHECK_LIST eType = (CLevel_MapTool::MODEL_CHECK_LIST)pGameObj->Get_ModelListType();

            if (CLevel_MapTool::MODEL_CHECK_LIST::CITY == eType ||
                CLevel_MapTool::MODEL_CHECK_LIST::BUILDING == eType)
            {
                continue;
            }           
        }

        CModel* pModel = dynamic_cast<CModel*>(pGameObj->Find_Component(L"Com_Model"));
        vector<CMesh*>& vecMeshs = pModel->Get_Meshes();

        for (size_t i = 0; i < vecMeshs.size(); i++)
        {
            if (vecMeshs[i]->isPicking(pGameObj->Get_Transform()->Get_WorldMatrix(), &vPickPos))
            {
                // �ش� ���� �޽��� �ϳ��� ��ŷ�� �ߴٸ�

                *pOut = pGameObj;
                return true;            // ������Ʈ �켱���� ��ŷ�ߴ�? ����� ��ŷ�������� �ͷ��� ����� 
            }          
        }
    }


    list<CGameObject*>& StaticObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Static_Object");
    for (auto pGameObj : StaticObjects)
    {
        if (true == g_PickingCity)
        {
            CLevel_MapTool::MODEL_CHECK_LIST eType = (CLevel_MapTool::MODEL_CHECK_LIST)pGameObj->Get_ModelListType();

            if (CLevel_MapTool::MODEL_CHECK_LIST::CITY == eType ||
                CLevel_MapTool::MODEL_CHECK_LIST::BUILDING == eType)
            {
                continue;
            }
        }

        CModel* pModel = dynamic_cast<CModel*>(pGameObj->Find_Component(L"Com_Model"));
        vector<CMesh*>& vecMeshs = pModel->Get_Meshes();

        for (size_t i = 0; i < vecMeshs.size(); i++)
        {
            if (vecMeshs[i]->isPicking(pGameObj->Get_Transform()->Get_WorldMatrix(), &vPickPos))
            {
                // �ش� ���� �޽��� �ϳ��� ��ŷ�� �ߴٸ�

                *pOut = pGameObj;
                return true;            // ������Ʈ �켱���� ��ŷ�ߴ�? ����� ��ŷ�������� �ͷ��� ����� 
            }
        }
    }


   







  

    

   
    if (false == g_PickingTerrain)          // City Picking ����  ��ŷ üũ X 
    {
        list<CGameObject*>& Terrains = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_BackGround_Terrain");

        _float3 vPos = {};

        for (auto pTerrain : Terrains)
        {
            if (dynamic_cast<CVIBuffer_Terrain*>(pTerrain->Find_Component(L"Com_VIBuffer"))->isPick(pTerrain->Get_Transform(), &vPos))
            {
                *pOut = pTerrain;
                return true;
            }
        }
    }

    

    return false;
}





HRESULT CLevel_MapTool::Ready_Object()
{  
    // ������Ʈ�� ���� ex) ����ƽ����, ���̳��Ϳ���, ����, ��Ÿ ���..

    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Decorative_Object"),
        CDecorative_Object::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Static_Object"),
        CStatic_Object::Create(m_pDevice, m_pContext))))
        return E_FAIL;
   
    if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_Object"),
        CDynamic_Object::Create(m_pDevice, m_pContext))))
        return E_FAIL;






    return S_OK;
}




void CLevel_MapTool::Save_Map()
{
    Save_Terrain();


    Save_NonAnimObject();
    Save_NonAnimObject_MapTool();


    Save_AnimObject();
    Save_AnimObject_MapTool();


    Save_Player();


    Save_OtherModel();
    Save_FinalMap();

}

void CLevel_MapTool::Save_Terrain()
{
    FILE* fout = fopen("../Bin/Map_Data.bin", "wb");
    if (!fout)  // ������ ���� ���ߴٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }


    list<CGameObject*>& TerrainList = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, TEXT("Layer_BackGround_Terrain"));
    _uint iTerrainSize = TerrainList.size();

    fwrite(&iTerrainSize, sizeof(iTerrainSize), 1, fout);

    for (auto pGameObj : TerrainList)
    {
        CTerrain* pTerrain = static_cast<CTerrain*>(pGameObj);

        _uint iNumX = pTerrain->Get_iNumX();
        _uint iNumZ = pTerrain->Get_iNumZ();
        _uint iTextureIndex = pTerrain->Get_TextureIndex();

        fwrite(&iNumX, sizeof(iNumX), 1, fout);
        fwrite(&iNumZ, sizeof(iNumZ), 1, fout);
        fwrite(&iTextureIndex, sizeof(iTextureIndex), 1, fout);

        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pTerrain->Get_Transform()->Get_WorldMatrix());

        fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);
    }

    fclose(fout);
    MSG_BOX(TEXT("Terrain ���� ����!"));
    // ===========================================================================================
}

void CLevel_MapTool::Save_NonAnimObject()
{  
    FILE* fout = fopen("../Bin/NonAnim_Model_Data.bin", "wb");
    if (!fout)    // ���� ���⿡ �����ߴٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }


    list<CGameObject*>& DecorativeObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Decorative_Object");

    _uint iObjectCount = DecorativeObjects.size();
    fwrite(&iObjectCount, sizeof(iObjectCount), 1, fout);  // ������Ʈ�� ���� ����

    for (auto pGameObj : DecorativeObjects)
    {
        _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
        _wstring wstrMyLayerName = pGameObj->Get_LayerName();
        CModel* pModel = pGameObj->Get_Model();

        // GameObject ����
        _uint objectType = pGameObj->Get_ModelListType();
        fwrite(&objectType, sizeof(objectType), 1, fout);

        std::string prototypeName = wstringToString(wstrMyPrototypeName);
        std::string layerName = wstringToString(wstrMyLayerName);

        _uint prototypeNameSize = prototypeName.size();
        _uint layerNameSize = layerName.size();

        fwrite(&prototypeNameSize, sizeof(prototypeNameSize), 1, fout);
        fwrite(prototypeName.c_str(), prototypeNameSize, 1, fout);

        fwrite(&layerNameSize, sizeof(layerNameSize), 1, fout);
        fwrite(layerName.c_str(), layerNameSize, 1, fout);

        // Model ����
        std::string modelPrototypeName = wstringToString(pModel->Get_PrototypeName());

        _uint modelType = pModel->Get_ModelType();
        std::string modelFilePath = wstringToString(pModel->Get_m_strModelFilePath());

        _uint modelPrototypeNameSize = modelPrototypeName.size();
        _uint modelFilePathSize = modelFilePath.size();

        _uint meshCount = pModel->Get_MeshesCount();
        _uint materialsCount = pModel->Get_MaterialsCount();

        _float4x4 PreTransformMatrix = pModel->Get_PreTransformMatrix();
        fwrite(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fout);

        fwrite(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fout);
        fwrite(modelPrototypeName.c_str(), modelPrototypeNameSize, 1, fout);

        fwrite(&modelType, sizeof(modelType), 1, fout);

        fwrite(&modelFilePathSize, sizeof(modelFilePathSize), 1, fout); // fbx ���� ��� ũ��
        fwrite(modelFilePath.c_str(), modelFilePathSize, 1, fout);

        fwrite(&meshCount, sizeof(meshCount), 1, fout);
        fwrite(&materialsCount, sizeof(materialsCount), 1, fout);

        for (size_t i = 0; i < materialsCount; i++)
        {
            _wstring* pwstrMaterialTexturePath = pModel->Get_MaterialTexturePath(i);

            for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
            {
                std::string materialTexturePath = wstringToString(pwstrMaterialTexturePath[j]);
                _uint pathSize = materialTexturePath.size();
                if (pathSize == 0)
                {
                    std::string noPath = "NO_PATH";
                    _uint noPathSize = noPath.size();

                    fwrite(&noPathSize, sizeof(noPathSize), 1, fout);
                    fwrite(noPath.c_str(), noPathSize, 1, fout);
                }
                else
                {
                    fwrite(&pathSize, sizeof(pathSize), 1, fout);
                    fwrite(materialTexturePath.c_str(), pathSize, 1, fout);
                }
            }
        }

        vector<CMesh*>& vecMesh = pModel->Get_Meshes();
        for (size_t i = 0; i < vecMesh.size(); i++)
        {
            _uint materialIndex = vecMesh[i]->Get_MaterialIndex();
            _uint numVertices = vecMesh[i]->m_iNumVertices;
            _uint vertexStride = vecMesh[i]->m_iVertexStride;

            fwrite(&materialIndex, sizeof(materialIndex), 1, fout);
            fwrite(&numVertices, sizeof(numVertices), 1, fout);
            fwrite(&vertexStride, sizeof(vertexStride), 1, fout);

            VTXMESH* pVertices = vecMesh[i]->Get_Vertices();
            for (size_t j = 0; j < numVertices; j++)
            {
                fwrite(&pVertices[j].vPosition, sizeof(pVertices[j].vPosition), 1, fout);
                fwrite(&pVertices[j].vNormal, sizeof(pVertices[j].vNormal), 1, fout);
                fwrite(&pVertices[j].vTexcoord, sizeof(pVertices[j].vTexcoord), 1, fout);
                fwrite(&pVertices[j].vTangent, sizeof(pVertices[j].vTangent), 1, fout);
            }

            _uint numIndices = vecMesh[i]->m_iNumIndices;
            _uint indexStride = vecMesh[i]->m_iIndexStride;
            fwrite(&numIndices, sizeof(numIndices), 1, fout);
            fwrite(&indexStride, sizeof(indexStride), 1, fout);

            _uint* pIndices = vecMesh[i]->Get_Indices();
            fwrite(pIndices, sizeof(_uint), numIndices, fout);

            fwrite(&vecMesh[i]->m_eIndexFormat, sizeof(vecMesh[i]->m_eIndexFormat), 1, fout);
            fwrite(&vecMesh[i]->m_eTopology, sizeof(vecMesh[i]->m_eTopology), 1, fout);

            fwrite(&vecMesh[i]->Get_MinPos(), sizeof(vecMesh[i]->Get_MinPos()), 1, fout);
            fwrite(&vecMesh[i]->Get_MaxPos(), sizeof(vecMesh[i]->Get_MaxPos()), 1, fout);
        }

        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

        fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);
    }




    list<CGameObject*>& StaticObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Static_Object");

    iObjectCount = StaticObjects.size();
    fwrite(&iObjectCount, sizeof(iObjectCount), 1, fout);  // ������Ʈ�� ���� ����

    for (auto pGameObj : StaticObjects)
    {
        _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
        _wstring wstrMyLayerName = pGameObj->Get_LayerName();
        CModel* pModel = pGameObj->Get_Model();

        // GameObject ����
        _uint objectType = pGameObj->Get_ModelListType();
        fwrite(&objectType, sizeof(objectType), 1, fout);

        std::string prototypeName = wstringToString(wstrMyPrototypeName);
        std::string layerName = wstringToString(wstrMyLayerName);

        _uint prototypeNameSize = prototypeName.size();
        _uint layerNameSize = layerName.size();

        fwrite(&prototypeNameSize, sizeof(prototypeNameSize), 1, fout);
        fwrite(prototypeName.c_str(), prototypeNameSize, 1, fout);

        fwrite(&layerNameSize, sizeof(layerNameSize), 1, fout);
        fwrite(layerName.c_str(), layerNameSize, 1, fout);

        // Model ����
        std::string modelPrototypeName = wstringToString(pModel->Get_PrototypeName());

        _uint modelType = pModel->Get_ModelType();
        std::string modelFilePath = wstringToString(pModel->Get_m_strModelFilePath());

        _uint modelPrototypeNameSize = modelPrototypeName.size();
        _uint modelFilePathSize = modelFilePath.size();

        _uint meshCount = pModel->Get_MeshesCount();
        _uint materialsCount = pModel->Get_MaterialsCount();

        _float4x4 PreTransformMatrix = pModel->Get_PreTransformMatrix();
        fwrite(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fout);

        fwrite(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fout);
        fwrite(modelPrototypeName.c_str(), modelPrototypeNameSize, 1, fout);

        fwrite(&modelType, sizeof(modelType), 1, fout);

        fwrite(&modelFilePathSize, sizeof(modelFilePathSize), 1, fout); // fbx ���� ��� ũ��
        fwrite(modelFilePath.c_str(), modelFilePathSize, 1, fout);

        fwrite(&meshCount, sizeof(meshCount), 1, fout);
        fwrite(&materialsCount, sizeof(materialsCount), 1, fout);

        for (size_t i = 0; i < materialsCount; i++)
        {
            _wstring* pwstrMaterialTexturePath = pModel->Get_MaterialTexturePath(i);

            for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
            {
                std::string materialTexturePath = wstringToString(pwstrMaterialTexturePath[j]);
                _uint pathSize = materialTexturePath.size();
                if (pathSize == 0)
                {
                    std::string noPath = "NO_PATH";
                    _uint noPathSize = noPath.size();

                    fwrite(&noPathSize, sizeof(noPathSize), 1, fout);
                    fwrite(noPath.c_str(), noPathSize, 1, fout);
                }
                else
                {
                    fwrite(&pathSize, sizeof(pathSize), 1, fout);
                    fwrite(materialTexturePath.c_str(), pathSize, 1, fout);
                }
            }
        }

        vector<CMesh*>& vecMesh = pModel->Get_Meshes();
        for (size_t i = 0; i < vecMesh.size(); i++)
        {
            _uint materialIndex = vecMesh[i]->Get_MaterialIndex();
            _uint numVertices = vecMesh[i]->m_iNumVertices;
            _uint vertexStride = vecMesh[i]->m_iVertexStride;

            fwrite(&materialIndex, sizeof(materialIndex), 1, fout);
            fwrite(&numVertices, sizeof(numVertices), 1, fout);
            fwrite(&vertexStride, sizeof(vertexStride), 1, fout);

            VTXMESH* pVertices = vecMesh[i]->Get_Vertices();
            for (size_t j = 0; j < numVertices; j++)
            {
                fwrite(&pVertices[j].vPosition, sizeof(pVertices[j].vPosition), 1, fout);
                fwrite(&pVertices[j].vNormal, sizeof(pVertices[j].vNormal), 1, fout);
                fwrite(&pVertices[j].vTexcoord, sizeof(pVertices[j].vTexcoord), 1, fout);
                fwrite(&pVertices[j].vTangent, sizeof(pVertices[j].vTangent), 1, fout);
            }

            _uint numIndices = vecMesh[i]->m_iNumIndices;
            _uint indexStride = vecMesh[i]->m_iIndexStride;
            fwrite(&numIndices, sizeof(numIndices), 1, fout);
            fwrite(&indexStride, sizeof(indexStride), 1, fout);

            _uint* pIndices = vecMesh[i]->Get_Indices();
            fwrite(pIndices, sizeof(_uint), numIndices, fout);

            fwrite(&vecMesh[i]->m_eIndexFormat, sizeof(vecMesh[i]->m_eIndexFormat), 1, fout);
            fwrite(&vecMesh[i]->m_eTopology, sizeof(vecMesh[i]->m_eTopology), 1, fout);

            fwrite(&vecMesh[i]->Get_MinPos(), sizeof(vecMesh[i]->Get_MinPos()), 1, fout);
            fwrite(&vecMesh[i]->Get_MaxPos(), sizeof(vecMesh[i]->Get_MaxPos()), 1, fout);
        }

        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

        fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);
    }





    list<CGameObject*>& DynamicObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Dynamic_Object");

    iObjectCount = DynamicObjects.size();
    fwrite(&iObjectCount, sizeof(iObjectCount), 1, fout);  // ������Ʈ�� ���� ����

    for (auto pGameObj : DynamicObjects)
    {
        _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
        _wstring wstrMyLayerName = pGameObj->Get_LayerName();
        CModel* pModel = pGameObj->Get_Model();

        // GameObject ����
        _uint objectType = pGameObj->Get_ModelListType();
        fwrite(&objectType, sizeof(objectType), 1, fout);

        std::string prototypeName = wstringToString(wstrMyPrototypeName);
        std::string layerName = wstringToString(wstrMyLayerName);

        _uint prototypeNameSize = prototypeName.size();
        _uint layerNameSize = layerName.size();

        fwrite(&prototypeNameSize, sizeof(prototypeNameSize), 1, fout);
        fwrite(prototypeName.c_str(), prototypeNameSize, 1, fout);

        fwrite(&layerNameSize, sizeof(layerNameSize), 1, fout);
        fwrite(layerName.c_str(), layerNameSize, 1, fout);

        // Model ����
        std::string modelPrototypeName = wstringToString(pModel->Get_PrototypeName());

        _uint modelType = pModel->Get_ModelType();
        std::string modelFilePath = wstringToString(pModel->Get_m_strModelFilePath());

        _uint modelPrototypeNameSize = modelPrototypeName.size();
        _uint modelFilePathSize = modelFilePath.size();

        _uint meshCount = pModel->Get_MeshesCount();
        _uint materialsCount = pModel->Get_MaterialsCount();

        _float4x4 PreTransformMatrix = pModel->Get_PreTransformMatrix();
        fwrite(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fout);

        fwrite(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fout);
        fwrite(modelPrototypeName.c_str(), modelPrototypeNameSize, 1, fout);

        fwrite(&modelType, sizeof(modelType), 1, fout);

        fwrite(&modelFilePathSize, sizeof(modelFilePathSize), 1, fout); // fbx ���� ��� ũ��
        fwrite(modelFilePath.c_str(), modelFilePathSize, 1, fout);

        fwrite(&meshCount, sizeof(meshCount), 1, fout);
        fwrite(&materialsCount, sizeof(materialsCount), 1, fout);

        for (size_t i = 0; i < materialsCount; i++)
        {
            _wstring* pwstrMaterialTexturePath = pModel->Get_MaterialTexturePath(i);

            for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
            {
                std::string materialTexturePath = wstringToString(pwstrMaterialTexturePath[j]);
                _uint pathSize = materialTexturePath.size();
                if (pathSize == 0)
                {
                    std::string noPath = "NO_PATH";
                    _uint noPathSize = noPath.size();

                    fwrite(&noPathSize, sizeof(noPathSize), 1, fout);
                    fwrite(noPath.c_str(), noPathSize, 1, fout);
                }
                else
                {
                    fwrite(&pathSize, sizeof(pathSize), 1, fout);
                    fwrite(materialTexturePath.c_str(), pathSize, 1, fout);
                }
            }
        }

        vector<CMesh*>& vecMesh = pModel->Get_Meshes();
        for (size_t i = 0; i < vecMesh.size(); i++)
        {
            _uint materialIndex = vecMesh[i]->Get_MaterialIndex();
            _uint numVertices = vecMesh[i]->m_iNumVertices;
            _uint vertexStride = vecMesh[i]->m_iVertexStride;

            fwrite(&materialIndex, sizeof(materialIndex), 1, fout);
            fwrite(&numVertices, sizeof(numVertices), 1, fout);
            fwrite(&vertexStride, sizeof(vertexStride), 1, fout);

            VTXMESH* pVertices = vecMesh[i]->Get_Vertices();
            for (size_t j = 0; j < numVertices; j++)
            {
                fwrite(&pVertices[j].vPosition, sizeof(pVertices[j].vPosition), 1, fout);
                fwrite(&pVertices[j].vNormal, sizeof(pVertices[j].vNormal), 1, fout);
                fwrite(&pVertices[j].vTexcoord, sizeof(pVertices[j].vTexcoord), 1, fout);
                fwrite(&pVertices[j].vTangent, sizeof(pVertices[j].vTangent), 1, fout);
            }

            _uint numIndices = vecMesh[i]->m_iNumIndices;
            _uint indexStride = vecMesh[i]->m_iIndexStride;
            fwrite(&numIndices, sizeof(numIndices), 1, fout);
            fwrite(&indexStride, sizeof(indexStride), 1, fout);

            _uint* pIndices = vecMesh[i]->Get_Indices();
            fwrite(pIndices, sizeof(_uint), numIndices, fout);

            fwrite(&vecMesh[i]->m_eIndexFormat, sizeof(vecMesh[i]->m_eIndexFormat), 1, fout);
            fwrite(&vecMesh[i]->m_eTopology, sizeof(vecMesh[i]->m_eTopology), 1, fout);

            fwrite(&vecMesh[i]->Get_MinPos(), sizeof(vecMesh[i]->Get_MinPos()), 1, fout);
            fwrite(&vecMesh[i]->Get_MaxPos(), sizeof(vecMesh[i]->Get_MaxPos()), 1, fout);
        }

        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

        fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);
    }











    fclose(fout);
    MSG_BOX(TEXT("NonAnim������ ����!"));

}

void CLevel_MapTool::Save_AnimObject()
{
    FILE* fp = NULL;

    // Anim �� ���� //
    fp = fopen("../Bin/Anim_Model_Data.bin", "wb");
    if (fp == NULL)  // ���� ���⿡ ������ ���
    {
        MessageBox(NULL, TEXT("���� ���⸦ �����߾��.."), TEXT("Error"), MB_OK);
        return;
    }

    list<CGameObject*>& AnimObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Anim_Object");
    _uint iObjectCount = AnimObjects.size();

    fwrite(&iObjectCount, sizeof(_uint), 1, fp);  // (�÷��̾� ����) ������Ʈ�� ����

    for (auto pGameObj : AnimObjects)
    {
        _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
        _wstring wstrMyLayerName = pGameObj->Get_LayerName();
        CModel* pModel = pGameObj->Get_Model();

        // GameObject
        _uint objectType = pGameObj->Get_ModelListType();
        fwrite(&objectType, sizeof(_uint), 1, fp);

        std::string prototypeName = wstringToString(wstrMyPrototypeName);
        std::string layerName = wstringToString(wstrMyLayerName);

        _uint prototypeNameSize = prototypeName.size();
        _uint layerNameSize = layerName.size();

        fwrite(&prototypeNameSize, sizeof(_uint), 1, fp);
        fwrite(prototypeName.c_str(), sizeof(char), prototypeNameSize, fp);

        fwrite(&layerNameSize, sizeof(_uint), 1, fp);
        fwrite(layerName.c_str(), sizeof(char), layerNameSize, fp);

        // Model
        std::string modelPrototypeName = wstringToString(pModel->Get_PrototypeName());

        _uint modelType = pModel->Get_ModelType();

        std::string modelFilePath = wstringToString(pModel->Get_m_strModelFilePath());

        _uint modelPrototypeNameSize = modelPrototypeName.size();
        _uint modelFilePathSize = modelFilePath.size();

        _uint meshCount = pModel->Get_MeshesCount();
        _uint materialsCount = pModel->Get_MaterialsCount();

        _float4x4 PreTransformMatrix = pModel->Get_PreTransformMatrix();
        fwrite(&PreTransformMatrix, sizeof(_float4x4), 1, fp);

        fwrite(&modelPrototypeNameSize, sizeof(_uint), 1, fp);
        fwrite(modelPrototypeName.c_str(), sizeof(char), modelPrototypeNameSize, fp);

        fwrite(&modelType, sizeof(_uint), 1, fp);

        fwrite(&modelFilePathSize, sizeof(_uint), 1, fp);  // fbx
        fwrite(modelFilePath.c_str(), sizeof(char), modelFilePathSize, fp);

        fwrite(&meshCount, sizeof(_uint), 1, fp);
        fwrite(&materialsCount, sizeof(_uint), 1, fp);

        for (size_t i = 0; i < materialsCount; i++)
        {
            _wstring* pwstrMaterialTexturePath = pModel->Get_MaterialTexturePath(i);

            for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
            {
                std::string materialTexturePath = wstringToString(pwstrMaterialTexturePath[j]);
                _uint pathSize = materialTexturePath.size();
                if (pathSize == 0)
                {
                    std::string noPath = "NO_PATH";
                    _uint noPathSize = noPath.size();

                    fwrite(&noPathSize, sizeof(_uint), 1, fp);
                    fwrite(noPath.c_str(), sizeof(char), noPathSize, fp);
                }
                else
                {
                    fwrite(&pathSize, sizeof(_uint), 1, fp);
                    fwrite(materialTexturePath.c_str(), sizeof(char), pathSize, fp);
                }
            }
        }

        vector<CMesh*>& vecMesh = pModel->Get_Meshes();
        for (size_t i = 0; i < vecMesh.size(); i++)
        {
            _uint materialIndex = vecMesh[i]->Get_MaterialIndex();
            _uint numVertices = vecMesh[i]->m_iNumVertices;
            _uint vertexStride = vecMesh[i]->m_iVertexStride;

            fwrite(&materialIndex, sizeof(_uint), 1, fp);
            fwrite(&numVertices, sizeof(_uint), 1, fp);
            fwrite(&vertexStride, sizeof(_uint), 1, fp);

            VTXANIMMESH* pVertices = vecMesh[i]->Get_AnimVertices();
            for (size_t j = 0; j < numVertices; j++)
            {
                fwrite(&pVertices[j].vPosition, sizeof(pVertices[j].vPosition), 1, fp);
                fwrite(&pVertices[j].vNormal, sizeof(pVertices[j].vNormal), 1, fp);
                fwrite(&pVertices[j].vTexcoord, sizeof(pVertices[j].vTexcoord), 1, fp);
                fwrite(&pVertices[j].vTangent, sizeof(pVertices[j].vTangent), 1, fp);
                fwrite(&pVertices[j].vBlendIndices, sizeof(pVertices[j].vBlendIndices), 1, fp);
                fwrite(&pVertices[j].vBlendWeights, sizeof(pVertices[j].vBlendWeights), 1, fp);
            }

            _uint numIndices = vecMesh[i]->m_iNumIndices;
            _uint indexStride = vecMesh[i]->m_iIndexStride;
            fwrite(&numIndices, sizeof(_uint), 1, fp);
            fwrite(&indexStride, sizeof(_uint), 1, fp);

            _uint* pIndices = vecMesh[i]->Get_Indices();
            fwrite(pIndices, sizeof(_uint), numIndices, fp);

            fwrite(&vecMesh[i]->m_eIndexFormat, sizeof(vecMesh[i]->m_eIndexFormat), 1, fp);
            fwrite(&vecMesh[i]->m_eTopology, sizeof(vecMesh[i]->m_eTopology), 1, fp);

            fwrite(&vecMesh[i]->Get_MinPos(), sizeof(vecMesh[i]->Get_MinPos()), 1, fp);  // ���� ������ min, max�� 
            fwrite(&vecMesh[i]->Get_MaxPos(), sizeof(vecMesh[i]->Get_MaxPos()), 1, fp);  // ���� ������ min, max�� 

            // ==========================================================

            vector<CBone*>& Bones = pModel->Get_Bones();

            _uint iNumModelBones = Bones.size();

            fwrite(&iNumModelBones, sizeof(_uint), 1, fp);  // �� ���� �� �� ���� 

            for (size_t i = 0; i < iNumModelBones; i++)  // �� ���� �� �� ������ŭ �������鼭 ������ �����Ѵ�.
            {
                _int iParentBoneIndex = Bones[i]->Get_ParentBoneIndex();
                _uint iNumChildBone = Bones[i]->Get_NumChildBone();

                const _char* szName = Bones[i]->Get_Name();
                size_t nameLength = strlen(szName);  // ���ڿ��� ���̸� ����մϴ�.

                _float4x4 TransformationMatrix = {};
                _float4x4 CombinedTransformationMatrix = {};

                XMStoreFloat4x4(&TransformationMatrix, Bones[i]->Get_TransformationMatrix());
                XMStoreFloat4x4(&CombinedTransformationMatrix, Bones[i]->Get_CombinedTransformationMatrix());

                fwrite(&iParentBoneIndex, sizeof(_int), 1, fp);  // �� ���� �θ� ���ε���
                fwrite(&iNumChildBone, sizeof(_uint), 1, fp);  // �� ���� �ڽ� �� ����

                fwrite(&nameLength, sizeof(size_t), 1, fp);
                fwrite(szName, sizeof(char), nameLength, fp);  // �� ���� �̸�

                fwrite(&TransformationMatrix, sizeof(_float4x4), 1, fp);  // �� ���� Ʈ��������� 
                fwrite(&CombinedTransformationMatrix, sizeof(_float4x4), 1, fp);  // �� ���� ���� �Ĺ��ε� ��� 
            }

            const _char* szName = vecMesh[i]->Get_Name();
            size_t nameLength = strlen(szName);

            fwrite(&nameLength, sizeof(size_t), 1, fp);
            fwrite(szName, sizeof(char), nameLength, fp);  // �� ���� �̸�

            _uint iNum_AffectedBones = vecMesh[i]->Get_Num_Affected_Bones();
            fwrite(&iNum_AffectedBones, sizeof(_uint), 1, fp);  // �� �޽��� ������ �޴� ���� ����

            vector<_uint>& BoneIndices = vecMesh[i]->Get_Affected_BoneIndices();
            vector<_float4x4>& OffsetMatrices = vecMesh[i]->Get_OffsetMatrices();  // m_iNumBones ���� ������ �޴� ���� ������� �̰� �����Ǿ� ����

            for (size_t i = 0; i < iNum_AffectedBones; i++)  // �� ������ �´ٸ� �� ������� BoneIndices.size ���� ������� ���� 
            {
                _uint iIndex = BoneIndices[i];
                _float4x4 OffsetMatrix = OffsetMatrices[i];

                fwrite(&iIndex, sizeof(_uint), 1, fp);  // ���� ������ �޴� ������ �ε���
                fwrite(&OffsetMatrix, sizeof(_float4x4), 1, fp);  // ���� ������ �޴� ���� ������ŭ�� offsetMatrix
            }

            vector<CAnimation*>& Animations = pModel->Get_Animations();
            _uint iNumAnimation = Animations.size();

            fwrite(&iNumAnimation, sizeof(_uint), 1, fp);  // �� �ִϸ��̼� �� ����

            for (size_t i = 0; i < Animations.size(); i++)
            {
                _char* szName = Animations[i]->Get_Name();
                size_t nameLength = strlen(szName);
                _double Duration = Animations[i]->Get_Duration();
                _double SpeedPerSec = Animations[i]->Get_SpeedPerSec();

                fwrite(&nameLength, sizeof(size_t), 1, fp);
                fwrite(szName, sizeof(char), nameLength, fp);  // �ִϸ��̼� �̸� 

                fwrite(&Duration, sizeof(_double), 1, fp);  // �ش� �ִϸ��̼��� �ѷ�
                fwrite(&SpeedPerSec, sizeof(_double), 1, fp);  // �ش� �ִϸ��̼��� �ʴ� �ӵ�

                _uint iNumChannel = Animations[i]->Get_NumChannel();
                vector<CChannel*>& Channels = Animations[i]->Get_Channels();

                fwrite(&iNumChannel, sizeof(_uint), 1, fp);  // �ش� �ִϸ��̼��� ä�� ���� (�� ����?)

                for (size_t i = 0; i < Channels.size(); i++)
                {
                    _char* szName = Channels[i]->Get_Name();
                    size_t nameLength = strlen(szName);
                    _uint iBoneIndex = Channels[i]->Get_BoneIndex();
                    _uint iNumKeyFrame = Channels[i]->Get_NumKeyFrames();
                    vector<KEYFRAME>& KeyFrame = Channels[i]->Get_KeyFrames();

                    fwrite(&nameLength, sizeof(size_t), 1, fp);
                    fwrite(szName, sizeof(char), nameLength, fp);  // �� ���� �̸�

                    fwrite(&iBoneIndex, sizeof(_uint), 1, fp);  // �ش� ä���� �����ϴ� ���� �ε���

                    fwrite(&iNumKeyFrame, sizeof(_uint), 1, fp);  // �ش� �ִϸ��̼��� ä���� Ű ������ ����

                    for (size_t i = 0; i < KeyFrame.size(); i++)
                    {
                        _float3 vScale = KeyFrame[i].vScale;
                        _float4 vRotation = KeyFrame[i].vRotation;
                        _float3 vTranslation = KeyFrame[i].vTranslation;
                        _double vTrackPosition = KeyFrame[i].TrackPosition;

                        fwrite(&vScale, sizeof(_float3), 1, fp);
                        fwrite(&vRotation, sizeof(_float4), 1, fp);
                        fwrite(&vTranslation, sizeof(_float3), 1, fp);
                        fwrite(&vTrackPosition, sizeof(_double), 1, fp);
                    }
                }
            }
        }

        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

        fwrite(&WorldMatrix, sizeof(_float4x4), 1, fp);


        if (MODEL_CHECK_LIST::ELITE == objectType ||
            MODEL_CHECK_LIST::JETPACK == objectType ||
            MODEL_CHECK_LIST::PISTOL == objectType ||
            MODEL_CHECK_LIST::SNIPER == objectType ||
            MODEL_CHECK_LIST::HEL == objectType)
        {
            // �����̳� ������Ʈ���

            _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
            _wstring wstrMyLayerName = pGameObj->Get_LayerName();


            // GameObject
            _uint objectType = pGameObj->Get_ModelListType();
            fwrite(&objectType, sizeof(objectType), 1, fp);

            std::string prototypeName = wstringToString(wstrMyPrototypeName);
            std::string layerName = wstringToString(wstrMyLayerName);

            _uint prototypeNameSize = prototypeName.size();
            _uint layerNameSize = layerName.size();

            fwrite(&prototypeNameSize, sizeof(prototypeNameSize), 1, fp);
            fwrite(prototypeName.c_str(), prototypeNameSize, 1, fp);
            fwrite(&layerNameSize, sizeof(layerNameSize), 1, fp);
            fwrite(layerName.c_str(), layerNameSize, 1, fp);

            CContainerObject* pContainerObject = static_cast<CContainerObject*>(pGameObj);
            _uint iPartSize = pContainerObject->Get_PartSize();

            fwrite(&iPartSize, sizeof(iPartSize), 1, fp);

            for (_uint i = 0; i < pContainerObject->Get_PartSize(); i++)
            {
                CModel* pModel = pContainerObject->Get_Part(i)->Get_Model();            

 
                std::string modelPrototypeName = wstringToString(pModel->Get_PrototypeName());
                _uint modelType = pModel->Get_ModelType();
                std::string modelFilePath = wstringToString(pModel->Get_m_strModelFilePath());

                _uint modelPrototypeNameSize = modelPrototypeName.size();
                _uint modelFilePathSize = modelFilePath.size();
                _uint meshCount = pModel->Get_MeshesCount();
                _uint materialsCount = pModel->Get_MaterialsCount();

                _float4x4 PreTransformMatrix = pModel->Get_PreTransformMatrix();
                fwrite(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fp);

                fwrite(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fp);
                fwrite(modelPrototypeName.c_str(), modelPrototypeNameSize, 1, fp);
                fwrite(&modelType, sizeof(modelType), 1, fp);
                fwrite(&modelFilePathSize, sizeof(modelFilePathSize), 1, fp);
                fwrite(modelFilePath.c_str(), modelFilePathSize, 1, fp);
                fwrite(&meshCount, sizeof(meshCount), 1, fp);
                fwrite(&materialsCount, sizeof(materialsCount), 1, fp);

                for (size_t i = 0; i < materialsCount; i++) {
                    _wstring* pwstrMaterialTexturePath = pModel->Get_MaterialTexturePath(i);
                    for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++) {
                        std::string materialTexturePath = wstringToString(pwstrMaterialTexturePath[j]);
                        _uint pathSize = materialTexturePath.size();
                        if (pathSize == 0) {
                            std::string noPath = "NO_PATH";
                            _uint noPathSize = noPath.size();
                            fwrite(&noPathSize, sizeof(noPathSize), 1, fp);
                            fwrite(noPath.c_str(), noPathSize, 1, fp);
                        }
                        else {
                            fwrite(&pathSize, sizeof(pathSize), 1, fp);
                            fwrite(materialTexturePath.c_str(), pathSize, 1, fp);
                        }
                    }
                }

                vector<CMesh*>& vecMesh = pModel->Get_Meshes();
                for (size_t i = 0; i < vecMesh.size(); i++) {
                    _uint materialIndex = vecMesh[i]->Get_MaterialIndex();
                    _uint numVertices = vecMesh[i]->m_iNumVertices;
                    _uint vertexStride = vecMesh[i]->m_iVertexStride;

                    fwrite(&materialIndex, sizeof(materialIndex), 1, fp);
                    fwrite(&numVertices, sizeof(numVertices), 1, fp);
                    fwrite(&vertexStride, sizeof(vertexStride), 1, fp);

                    VTXMESH* pVertices = vecMesh[i]->Get_Vertices();
                    for (size_t j = 0; j < numVertices; j++) {
                        fwrite(&pVertices[j].vPosition, sizeof(pVertices[j].vPosition), 1, fp);
                        fwrite(&pVertices[j].vNormal, sizeof(pVertices[j].vNormal), 1, fp);
                        fwrite(&pVertices[j].vTexcoord, sizeof(pVertices[j].vTexcoord), 1, fp);
                        fwrite(&pVertices[j].vTangent, sizeof(pVertices[j].vTangent), 1, fp);
                    }

                    _uint numIndices = vecMesh[i]->m_iNumIndices;
                    _uint indexStride = vecMesh[i]->m_iIndexStride;
                    fwrite(&numIndices, sizeof(numIndices), 1, fp);
                    fwrite(&indexStride, sizeof(indexStride), 1, fp);

                    _uint* pIndices = vecMesh[i]->Get_Indices();
                    fwrite(pIndices, numIndices * sizeof(_uint), 1, fp);

                    fwrite(&vecMesh[i]->m_eIndexFormat, sizeof(vecMesh[i]->m_eIndexFormat), 1, fp);
                    fwrite(&vecMesh[i]->m_eTopology, sizeof(vecMesh[i]->m_eTopology), 1, fp);

                    fwrite(&vecMesh[i]->Get_MinPos(), sizeof(vecMesh[i]->Get_MinPos()), 1, fp);
                    fwrite(&vecMesh[i]->Get_MaxPos(), sizeof(vecMesh[i]->Get_MaxPos()), 1, fp);
                }

            }


            _float4x4 WorldMatrix = {};
            XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

            fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fp);

        }
                
            

    
    }

    fclose(fp);
    MessageBox(NULL, TEXT("Anim������ ����!"), TEXT("Success"), MB_OK);

}

void CLevel_MapTool::Save_NonAnimObject_MapTool()
{
    ofstream fout;

    // NonAnim �� ���� //
    fout.open("../Bin/NonAnim_Model_Data_MapTool.bin", ios::binary);
    if (!fout.is_open())    // ���� �����ٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }


    list<CGameObject*>& DecorativeObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Decorative_Object");

    _uint iObjectCount = DecorativeObjects.size();

    fout.write(reinterpret_cast<_char*>(&iObjectCount), sizeof(iObjectCount));                    // ������Ʈ�� ����

    for (auto pGameObj : DecorativeObjects)
    {
        _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
        _wstring wstrMyLayerName = pGameObj->Get_LayerName();
        CModel* pModel = pGameObj->Get_Model();


        // GameObject
        _uint objectType = pGameObj->Get_ModelListType();
        fout.write(reinterpret_cast<char*>(&objectType), sizeof(objectType));


        _uint ModelNum = pGameObj->Get_ModelNum();
        fout.write(reinterpret_cast<char*>(&ModelNum), sizeof(ModelNum));



        std::string prototypeName = wstringToString(wstrMyPrototypeName);
        std::string layerName = wstringToString(wstrMyLayerName);

        _uint prototypeNameSize = prototypeName.size();
        _uint layerNameSize = layerName.size();


        fout.write(reinterpret_cast<char*>(&prototypeNameSize), sizeof(prototypeNameSize));
        fout.write(prototypeName.c_str(), prototypeNameSize);


        fout.write(reinterpret_cast<char*>(&layerNameSize), sizeof(layerNameSize));
        fout.write(layerName.c_str(), layerNameSize);





        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

        fout.write(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix));
    }



    list<CGameObject*>& StaticObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Static_Object");

   iObjectCount = StaticObjects.size();

    fout.write(reinterpret_cast<_char*>(&iObjectCount), sizeof(iObjectCount));                    // ������Ʈ�� ����

    for (auto pGameObj : StaticObjects)
    {
        _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
        _wstring wstrMyLayerName = pGameObj->Get_LayerName();
        CModel* pModel = pGameObj->Get_Model();


        // GameObject
        _uint objectType = pGameObj->Get_ModelListType();
        fout.write(reinterpret_cast<char*>(&objectType), sizeof(objectType));

        _uint ModelNum = pGameObj->Get_ModelNum();
        fout.write(reinterpret_cast<char*>(&ModelNum), sizeof(ModelNum));



        std::string prototypeName = wstringToString(wstrMyPrototypeName);
        std::string layerName = wstringToString(wstrMyLayerName);

        _uint prototypeNameSize = prototypeName.size();
        _uint layerNameSize = layerName.size();


        fout.write(reinterpret_cast<char*>(&prototypeNameSize), sizeof(prototypeNameSize));
        fout.write(prototypeName.c_str(), prototypeNameSize);


        fout.write(reinterpret_cast<char*>(&layerNameSize), sizeof(layerNameSize));
        fout.write(layerName.c_str(), layerNameSize);





        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

        fout.write(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix));
    }





    list<CGameObject*>& DynamicObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Dynamic_Object");

    iObjectCount = DynamicObjects.size();

    fout.write(reinterpret_cast<_char*>(&iObjectCount), sizeof(iObjectCount));                    // ������Ʈ�� ����

    for (auto pGameObj : DynamicObjects)
    {
        _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
        _wstring wstrMyLayerName = pGameObj->Get_LayerName();
        CModel* pModel = pGameObj->Get_Model();


        // GameObject
        _uint objectType = pGameObj->Get_ModelListType();
        fout.write(reinterpret_cast<char*>(&objectType), sizeof(objectType));

        _uint ModelNum = pGameObj->Get_ModelNum();
        fout.write(reinterpret_cast<char*>(&ModelNum), sizeof(ModelNum));



        std::string prototypeName = wstringToString(wstrMyPrototypeName);
        std::string layerName = wstringToString(wstrMyLayerName);

        _uint prototypeNameSize = prototypeName.size();
        _uint layerNameSize = layerName.size();


        fout.write(reinterpret_cast<char*>(&prototypeNameSize), sizeof(prototypeNameSize));
        fout.write(prototypeName.c_str(), prototypeNameSize);


        fout.write(reinterpret_cast<char*>(&layerNameSize), sizeof(layerNameSize));
        fout.write(layerName.c_str(), layerNameSize);





        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

        fout.write(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix));
    }







    fout.close();
    MSG_BOX(TEXT("�������� NonAnim������ ����!"));


}

void CLevel_MapTool::Save_AnimObject_MapTool()
{
    ofstream fout;

    // NonAnim �� ���� //
    fout.open("../Bin/Anim_Model_Data_MapTool.bin", ios::binary);
    if (!fout.is_open())    // ���� �����ٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }

    
    // ���ʿ� �÷��̾�� ���̾ �޶� �÷��̾ ������ �ֵ��� �������ֳ�
    list<CGameObject*>& GameObjects = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_Anim_Object");

    _uint iObjectCount = GameObjects.size();

    fout.write(reinterpret_cast<_char*>(&iObjectCount), sizeof(iObjectCount));                    // ������Ʈ�� ����

    for (auto pGameObj : GameObjects)
    {
       
        _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
        _wstring wstrMyLayerName = pGameObj->Get_LayerName();
        CModel* pModel = pGameObj->Get_Model();


        // GameObject
        _uint objectType = pGameObj->Get_ModelListType();
        fout.write(reinterpret_cast<char*>(&objectType), sizeof(objectType));

        _uint ModelNum = pGameObj->Get_ModelNum();
        fout.write(reinterpret_cast<char*>(&ModelNum), sizeof(ModelNum));



        std::string prototypeName = wstringToString(wstrMyPrototypeName);
        std::string layerName = wstringToString(wstrMyLayerName);

        _uint prototypeNameSize = prototypeName.size();
        _uint layerNameSize = layerName.size();


        fout.write(reinterpret_cast<char*>(&prototypeNameSize), sizeof(prototypeNameSize));
        fout.write(prototypeName.c_str(), prototypeNameSize);


        fout.write(reinterpret_cast<char*>(&layerNameSize), sizeof(layerNameSize));
        fout.write(layerName.c_str(), layerNameSize);





        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

        fout.write(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix));
    }

    fout.close();
    MSG_BOX(TEXT("�������� Anim������ ����!"));
}

void CLevel_MapTool::Save_Player()
{
    FILE* fout = fopen("../Bin/Player_Model_Data.bin", "wb");

    if (fout == nullptr) {
        perror("���� ���� ����");
        return;
    }

    CGameObject* pGameObj = m_pGameInstance->Find_Player();

    _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
    _wstring wstrMyLayerName = pGameObj->Get_LayerName();

    CModel* pBodyModel = dynamic_cast<CContainerObject*>(pGameObj)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

    // GameObject
    _uint objectType = pGameObj->Get_ModelListType();
    fwrite(&objectType, sizeof(_uint), 1, fout);

    std::string prototypeName = wstringToString(wstrMyPrototypeName);
    std::string layerName = wstringToString(wstrMyLayerName);

    _uint prototypeNameSize = prototypeName.size();
    _uint layerNameSize = layerName.size();

    fwrite(&prototypeNameSize, sizeof(prototypeNameSize), 1, fout);
    fwrite(prototypeName.c_str(), prototypeNameSize, 1, fout);

    fwrite(&layerNameSize, sizeof(layerNameSize), 1, fout);
    fwrite(layerName.c_str(), layerNameSize, 1, fout);

    // Model
    std::string modelPrototypeName = wstringToString(pBodyModel->Get_PrototypeName());

    _uint modelType = pBodyModel->Get_ModelType();
    std::string modelFilePath = wstringToString(pBodyModel->Get_m_strModelFilePath());

    _uint modelPrototypeNameSize = modelPrototypeName.size();
    _uint modelFilePathSize = modelFilePath.size();
    _uint meshCount = pBodyModel->Get_MeshesCount();
    _uint materialsCount = pBodyModel->Get_MaterialsCount();

    _float4x4 PreTransformMatrix = pBodyModel->Get_PreTransformMatrix();

    fwrite(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fout);
    fwrite(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fout);
    fwrite(modelPrototypeName.c_str(), modelPrototypeNameSize, 1, fout);
    fwrite(&modelType, sizeof(modelType), 1, fout);
    fwrite(&modelFilePathSize, sizeof(modelFilePathSize), 1, fout);
    fwrite(modelFilePath.c_str(), modelFilePathSize, 1, fout);
    fwrite(&meshCount, sizeof(meshCount), 1, fout);
    fwrite(&materialsCount, sizeof(materialsCount), 1, fout);

    for (size_t i = 0; i < materialsCount; i++) {
        _wstring* pwstrMaterialTexturePath = pBodyModel->Get_MaterialTexturePath(i);
        for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++) {
            std::string materialTexturePath = wstringToString(pwstrMaterialTexturePath[j]);
            _uint pathSize = materialTexturePath.size();
            if (pathSize == 0) {
                std::string noPath = "NO_PATH";
                _uint noPathSize = noPath.size();
                fwrite(&noPathSize, sizeof(noPathSize), 1, fout);
                fwrite(noPath.c_str(), noPathSize, 1, fout);
            }
            else {
                fwrite(&pathSize, sizeof(pathSize), 1, fout);
                fwrite(materialTexturePath.c_str(), pathSize, 1, fout);
            }
        }
    }

    vector<CMesh*>& vecMesh = pBodyModel->Get_Meshes();
    for (size_t i = 0; i < vecMesh.size(); i++) {
        _uint materialIndex = vecMesh[i]->Get_MaterialIndex();
        _uint numVertices = vecMesh[i]->m_iNumVertices;
        _uint vertexStride = vecMesh[i]->m_iVertexStride;

        fwrite(&materialIndex, sizeof(materialIndex), 1, fout);
        fwrite(&numVertices, sizeof(numVertices), 1, fout);
        fwrite(&vertexStride, sizeof(vertexStride), 1, fout);

        VTXANIMMESH* pVertices = vecMesh[i]->Get_AnimVertices();
        for (size_t j = 0; j < numVertices; j++) {
            fwrite(&pVertices[j].vPosition, sizeof(pVertices[j].vPosition), 1, fout);
            fwrite(&pVertices[j].vNormal, sizeof(pVertices[j].vNormal), 1, fout);
            fwrite(&pVertices[j].vTexcoord, sizeof(pVertices[j].vTexcoord), 1, fout);
            fwrite(&pVertices[j].vTangent, sizeof(pVertices[j].vTangent), 1, fout);
            fwrite(&pVertices[j].vBlendIndices, sizeof(pVertices[j].vBlendIndices), 1, fout);
            fwrite(&pVertices[j].vBlendWeights, sizeof(pVertices[j].vBlendWeights), 1, fout);
        }

        _uint numIndices = vecMesh[i]->m_iNumIndices;
        _uint indexStride = vecMesh[i]->m_iIndexStride;
        fwrite(&numIndices, sizeof(numIndices), 1, fout);
        fwrite(&indexStride, sizeof(indexStride), 1, fout);

        _uint* pIndices = vecMesh[i]->Get_Indices();
        fwrite(pIndices, numIndices * sizeof(_uint), 1, fout);

        fwrite(&vecMesh[i]->m_eIndexFormat, sizeof(vecMesh[i]->m_eIndexFormat), 1, fout);
        fwrite(&vecMesh[i]->m_eTopology, sizeof(vecMesh[i]->m_eTopology), 1, fout);

        fwrite(&vecMesh[i]->Get_MinPos(), sizeof(vecMesh[i]->Get_MinPos()), 1, fout);
        fwrite(&vecMesh[i]->Get_MaxPos(), sizeof(vecMesh[i]->Get_MaxPos()), 1, fout);

        // ==========================================================

        vector<CBone*>& Bones = pBodyModel->Get_Bones();
        _uint iNumModelBones = Bones.size();

        fwrite(&iNumModelBones, sizeof(iNumModelBones), 1, fout);

        for (size_t i = 0; i < iNumModelBones; i++) {
            _int iParentBoneIndex = Bones[i]->Get_ParentBoneIndex();
            _uint iNumChildBone = Bones[i]->Get_NumChildBone();
            const _char* szName = Bones[i]->Get_Name();
            size_t nameLength = strlen(szName);

            _float4x4 TransformationMatrix = {};
            _float4x4 CombinedTransformationMatrix = {};

            XMStoreFloat4x4(&TransformationMatrix, Bones[i]->Get_TransformationMatrix());
            XMStoreFloat4x4(&CombinedTransformationMatrix, Bones[i]->Get_CombinedTransformationMatrix());

            fwrite(&iParentBoneIndex, sizeof(iParentBoneIndex), 1, fout);
            fwrite(&iNumChildBone, sizeof(iNumChildBone), 1, fout);
            fwrite(&nameLength, sizeof(nameLength), 1, fout);
            fwrite(szName, nameLength, 1, fout);
            fwrite(&TransformationMatrix, sizeof(TransformationMatrix), 1, fout);
            fwrite(&CombinedTransformationMatrix, sizeof(CombinedTransformationMatrix), 1, fout);
        }

        const _char* szName = vecMesh[i]->Get_Name();
        size_t nameLength = strlen(szName);

        fwrite(&nameLength, sizeof(nameLength), 1, fout);
        fwrite(szName, nameLength, 1, fout);

        _uint iNum_AffectedBones = vecMesh[i]->Get_Num_Affected_Bones();

        fwrite(&iNum_AffectedBones, sizeof(iNum_AffectedBones), 1, fout);

        vector<_uint>& BoneIndices = vecMesh[i]->Get_Affected_BoneIndices();
        vector<_float4x4>& OffsetMatrices = vecMesh[i]->Get_OffsetMatrices();

        for (size_t i = 0; i < iNum_AffectedBones; i++) {
            _uint iIndex = BoneIndices[i];
            _float4x4 OffsetMatrix = OffsetMatrices[i];

            fwrite(&iIndex, sizeof(iIndex), 1, fout);
            fwrite(&OffsetMatrix, sizeof(OffsetMatrix), 1, fout);
        }

        vector<CAnimation*>& Animations = pBodyModel->Get_Animations();
        _uint iNumAnimation = Animations.size();

        fwrite(&iNumAnimation, sizeof(iNumAnimation), 1, fout);

        for (size_t i = 0; i < Animations.size(); i++) {
            _char* szName = Animations[i]->Get_Name();
            size_t nameLength = strlen(szName);
            _double Duration = Animations[i]->Get_Duration();
            _double SpeedPerSec = Animations[i]->Get_SpeedPerSec();

            fwrite(&nameLength, sizeof(nameLength), 1, fout);
            fwrite(szName, nameLength, 1, fout);
            fwrite(&Duration, sizeof(Duration), 1, fout);
            fwrite(&SpeedPerSec, sizeof(SpeedPerSec), 1, fout);

            _uint iNumChannel = Animations[i]->Get_NumChannel();
            vector<CChannel*>& Channels = Animations[i]->Get_Channels();

            fwrite(&iNumChannel, sizeof(iNumChannel), 1, fout);

            for (size_t i = 0; i < Channels.size(); i++) {
                _char* szName = Channels[i]->Get_Name();
                size_t nameLength = strlen(szName);
                _uint iBoneIndex = Channels[i]->Get_BoneIndex();
                _uint iNumKeyFrame = Channels[i]->Get_NumKeyFrames();
                vector<KEYFRAME>& KeyFrame = Channels[i]->Get_KeyFrames();

                fwrite(&nameLength, sizeof(nameLength), 1, fout);
                fwrite(szName, nameLength, 1, fout);
                fwrite(&iBoneIndex, sizeof(iBoneIndex), 1, fout);
                fwrite(&iNumKeyFrame, sizeof(iNumKeyFrame), 1, fout);

                for (size_t i = 0; i < KeyFrame.size(); i++) {
                    _float3 vScale = KeyFrame[i].vScale;
                    _float4 vRotation = KeyFrame[i].vRotation;
                    _float3 vTranslation = KeyFrame[i].vTranslation;
                    _double vTrackPosition = KeyFrame[i].TrackPosition;

                    fwrite(&vScale, sizeof(vScale), 1, fout);
                    fwrite(&vRotation, sizeof(vRotation), 1, fout);
                    fwrite(&vTranslation, sizeof(vTranslation), 1, fout);
                    fwrite(&vTrackPosition, sizeof(vTrackPosition), 1, fout);
                }
            }
        }

        _float4x4 WorldMatrix = {};
        XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

        fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);

        // Player Weapon
        {
            _wstring wstrMyPrototypeName = pGameObj->Get_PrototypeName();
            _wstring wstrMyLayerName = pGameObj->Get_LayerName();

                  
            // GameObject
            _uint objectType = pGameObj->Get_ModelListType();
            fwrite(&objectType, sizeof(objectType), 1, fout);

            std::string prototypeName = wstringToString(wstrMyPrototypeName);
            std::string layerName = wstringToString(wstrMyLayerName);

            _uint prototypeNameSize = prototypeName.size();
            _uint layerNameSize = layerName.size();

            fwrite(&prototypeNameSize, sizeof(prototypeNameSize), 1, fout);
            fwrite(prototypeName.c_str(), prototypeNameSize, 1, fout);
            fwrite(&layerNameSize, sizeof(layerNameSize), 1, fout);
            fwrite(layerName.c_str(), layerNameSize, 1, fout);



            for (_uint i = 0; i < CWeapon_Player::WEAPON_TYPE_END; i++)
            {
                CPartObject* pPartWeapon = dynamic_cast<CContainerObject*>(pGameObj)->Get_Part(CPlayer::PARTID::PART_WEAPON);

                CModel* pWeaponModel = dynamic_cast<CWeapon_Player*>(pPartWeapon)->Get_TypeModel(CWeapon_Player::WEAPON_TYPE(i));


                // Model
                std::string modelPrototypeName = wstringToString(pWeaponModel->Get_PrototypeName());
                _uint modelType = pWeaponModel->Get_ModelType();
                std::string modelFilePath = wstringToString(pWeaponModel->Get_m_strModelFilePath());

                _uint modelPrototypeNameSize = modelPrototypeName.size();
                _uint modelFilePathSize = modelFilePath.size();
                _uint meshCount = pWeaponModel->Get_MeshesCount();
                _uint materialsCount = pWeaponModel->Get_MaterialsCount();

                _float4x4 PreTransformMatrix = pWeaponModel->Get_PreTransformMatrix();
                fwrite(&PreTransformMatrix, sizeof(PreTransformMatrix), 1, fout);

                fwrite(&modelPrototypeNameSize, sizeof(modelPrototypeNameSize), 1, fout);
                fwrite(modelPrototypeName.c_str(), modelPrototypeNameSize, 1, fout);
                fwrite(&modelType, sizeof(modelType), 1, fout);
                fwrite(&modelFilePathSize, sizeof(modelFilePathSize), 1, fout);
                fwrite(modelFilePath.c_str(), modelFilePathSize, 1, fout);
                fwrite(&meshCount, sizeof(meshCount), 1, fout);
                fwrite(&materialsCount, sizeof(materialsCount), 1, fout);

                for (size_t i = 0; i < materialsCount; i++) {
                    _wstring* pwstrMaterialTexturePath = pWeaponModel->Get_MaterialTexturePath(i);
                    for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++) {
                        std::string materialTexturePath = wstringToString(pwstrMaterialTexturePath[j]);
                        _uint pathSize = materialTexturePath.size();
                        if (pathSize == 0) {
                            std::string noPath = "NO_PATH";
                            _uint noPathSize = noPath.size();
                            fwrite(&noPathSize, sizeof(noPathSize), 1, fout);
                            fwrite(noPath.c_str(), noPathSize, 1, fout);
                        }
                        else {
                            fwrite(&pathSize, sizeof(pathSize), 1, fout);
                            fwrite(materialTexturePath.c_str(), pathSize, 1, fout);
                        }
                    }
                }

                vector<CMesh*>& vecMesh = pWeaponModel->Get_Meshes();
                for (size_t i = 0; i < vecMesh.size(); i++) {
                    _uint materialIndex = vecMesh[i]->Get_MaterialIndex();
                    _uint numVertices = vecMesh[i]->m_iNumVertices;
                    _uint vertexStride = vecMesh[i]->m_iVertexStride;

                    fwrite(&materialIndex, sizeof(materialIndex), 1, fout);
                    fwrite(&numVertices, sizeof(numVertices), 1, fout);
                    fwrite(&vertexStride, sizeof(vertexStride), 1, fout);

                    VTXMESH* pVertices = vecMesh[i]->Get_Vertices();
                    for (size_t j = 0; j < numVertices; j++) {
                        fwrite(&pVertices[j].vPosition, sizeof(pVertices[j].vPosition), 1, fout);
                        fwrite(&pVertices[j].vNormal, sizeof(pVertices[j].vNormal), 1, fout);
                        fwrite(&pVertices[j].vTexcoord, sizeof(pVertices[j].vTexcoord), 1, fout);
                        fwrite(&pVertices[j].vTangent, sizeof(pVertices[j].vTangent), 1, fout);
                    }

                    _uint numIndices = vecMesh[i]->m_iNumIndices;
                    _uint indexStride = vecMesh[i]->m_iIndexStride;
                    fwrite(&numIndices, sizeof(numIndices), 1, fout);
                    fwrite(&indexStride, sizeof(indexStride), 1, fout);

                    _uint* pIndices = vecMesh[i]->Get_Indices();
                    fwrite(pIndices, numIndices * sizeof(_uint), 1, fout);

                    fwrite(&vecMesh[i]->m_eIndexFormat, sizeof(vecMesh[i]->m_eIndexFormat), 1, fout);
                    fwrite(&vecMesh[i]->m_eTopology, sizeof(vecMesh[i]->m_eTopology), 1, fout);

                    fwrite(&vecMesh[i]->Get_MinPos(), sizeof(vecMesh[i]->Get_MinPos()), 1, fout);
                    fwrite(&vecMesh[i]->Get_MaxPos(), sizeof(vecMesh[i]->Get_MaxPos()), 1, fout);
                }

            }

            

            _float4x4 WorldMatrix = {};
            XMStoreFloat4x4(&WorldMatrix, pGameObj->Get_Transform()->Get_WorldMatrix());

            fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);
        }

        fclose(fout);
        MSG_BOX(TEXT("Player ���� ����!"));

    }
}

void CLevel_MapTool::Save_Navigation()
{
    ofstream fout;

    // �ͷ��� ����//
    fout.open("../Bin/Navigation_Data.bin", ios::binary);
    if (!fout.is_open())    // ���� �����ٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }


    // ������ �׺���̼� ?
    list<CGameObject*>& Terrians = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, TEXT("Layer_BackGround_Terrain"));

    for (auto& Terrain : Terrians)
    {
        CNavigation* pNavigation = Terrain->Get_Navigation();
        vector<CCell*>& Cells = pNavigation->Get_Cells();


        _uint iCellSize = Cells.size();

        fout.write(reinterpret_cast<_char*>(&iCellSize), sizeof(iCellSize));

        for (_uint i = 0; i < Cells.size(); i++)
        {
            _int       iIndex = Cells[i]->Get_Index();

            _float3     PointA = {};
            _float3     PointB = {};
            _float3     PointC = {};

            _int     NeighborsIndexAB = {};
            _int     NeighborsIndexBC = {};
            _int     NeighborsIndexCA = {};

            XMStoreFloat3(&PointA, Cells[i]->Get_Point(CCell::POINT_A));
            XMStoreFloat3(&PointB, Cells[i]->Get_Point(CCell::POINT_B));
            XMStoreFloat3(&PointC, Cells[i]->Get_Point(CCell::POINT_C));
              
            NeighborsIndexAB = *Cells[i]->Get_NeighborIndices();
            NeighborsIndexBC = *(Cells[i]->Get_NeighborIndices()+1);
            NeighborsIndexCA = *(Cells[i]->Get_NeighborIndices()+2);


            fout.write(reinterpret_cast<_char*>(&iIndex), sizeof(iIndex));

            fout.write(reinterpret_cast<_char*>(&PointA), sizeof(PointA));
            fout.write(reinterpret_cast<_char*>(&PointB), sizeof(PointB));
            fout.write(reinterpret_cast<_char*>(&PointC), sizeof(PointC));

            fout.write(reinterpret_cast<_char*>(&NeighborsIndexAB), sizeof(NeighborsIndexAB));
            fout.write(reinterpret_cast<_char*>(&NeighborsIndexBC), sizeof(NeighborsIndexBC));
            fout.write(reinterpret_cast<_char*>(&NeighborsIndexCA), sizeof(NeighborsIndexCA));
        }
    }


    fout.close();
    MSG_BOX(TEXT("Navigation ���� ����!"));
}

void CLevel_MapTool::Load_Map()
{
    //Load_Terrain();

    //Load_Final_Map_MapTool();           // 3dMax���� ������ ���� �ϳ��� �ʿ�����Ʈ ��ü ��ο��� 1���� ����

    Load_NonAnimObject_MapTool();       // �ΰ����� ���̾ Ÿ������ ������Ʈ, Ŭ���̹��� �ϱ����� ������Ʈ ��ġ

    Load_AnimObject_MapTool();          // �׿� ���͵�

}


void CLevel_MapTool::Load_Terrain()
{
    FILE* fin = fopen("../Bin/Map_Data.bin", "rb");
    if (fin == nullptr) {
        // ���� ���⿡ ������ ���
        perror("���� ���� ����");
        return;
    }

    _uint iNumTerrain = 0;
    _uint iVerticesX = 0;
    _uint iVerticesZ = 0;
    _uint iTextureIndex = 0;

    // iNumTerrain �б�
    fread(&iNumTerrain, sizeof(iNumTerrain), 1, fin);

    for (size_t i = 0; i < iNumTerrain; i++) {
        // ������ �б�
        fread(&iVerticesX, sizeof(iVerticesX), 1, fin);
        fread(&iVerticesZ, sizeof(iVerticesZ), 1, fin);
        fread(&iTextureIndex, sizeof(iTextureIndex), 1, fin);

        _float4x4 WorldMatrix;
        fread(&WorldMatrix, sizeof(WorldMatrix), 1, fin);

        // For. Prototype_Component_VIBuffer_Terrain
        std::wstring TerrainBuffer_Prototype = L"Prototype_Component_VIBuffer_Terrain_";
        TerrainBuffer_Prototype += std::to_wstring(iVerticesX) + L"_" + std::to_wstring(iVerticesZ);

        if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TerrainBuffer_Prototype.c_str(),
            CVIBuffer_Terrain::Create(m_pDevice, m_pContext, iVerticesX, iVerticesZ, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp"))))) {
            fclose(fin);
            return;
        }

        // For. Prototype_GameObject_Terrain
        std::wstring Terrain_Prototype = L"Prototype_GameObject_Terrain_";
        Terrain_Prototype += std::to_wstring(iVerticesX) + L"_" + std::to_wstring(iVerticesZ);

        if (FAILED(m_pGameInstance->Add_Prototype(Terrain_Prototype.c_str(),
            CTerrain::Create(m_pDevice, m_pContext, iVerticesX, iVerticesZ)))) {
            fclose(fin);
            return;
        }

        CGameObject::GAMEOBJECT_DESC Desc = {};
        Desc.InitWorldMatrix = XMLoadFloat4x4(&WorldMatrix);

        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, TEXT("Layer_BackGround_Terrain"), Terrain_Prototype.c_str(), &Desc))) {
            fclose(fin);
            return;
        }
    }

    fclose(fin);
    MSG_BOX(TEXT("LEVEL_MAPTOOL�� Terrain ������ Load�߽��ϴ�."));

}


void CLevel_MapTool::Load_NonAnimObject_MapTool()
{

    ifstream fin;

    fin.open("../Bin/NonAnim_Model_Data_MapTool.bin", ios::binary);
    if (!fin.is_open())    // ���� �����ٸ�
    {
        MSG_BOX(TEXT("�������� NonAnim������ �����ϴ�\n���ð����� �޼����Դϴ�."));
        return;
    }

    _uint iObjectCount = {};

    _uint iObjectType = {};
    _uint iModelNum = {};
   

    string strMyPrototypeName = "";
    wstring wstrMyPrototypeName = L"";

    string strMyLayerName = "";
    wstring wstrMyLayerName = L"";

    fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(iObjectCount));


    Open_ObjectDialog();            // ������ Ready_Object�� ������Ÿ���� �������

    CGameObject::GAMEOBJECT_DESC Desc = {};
  
    for (_uint i = 0; i < iObjectCount; i++)
    {
        // ��ü���� //
        fin.read(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType));
        fin.read(reinterpret_cast<char*>(&iModelNum), sizeof(iModelNum));

        // ������Ÿ�� �̸� �б�
        ReadString(fin, strMyPrototypeName);
        wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

        // ���̾� �̸� �б�
        ReadString(fin, strMyLayerName);
        wstrMyLayerName = stringToWstring(strMyLayerName);


        // ���� ��� �б�
        _float4x4 WorldMatrix;
        fin.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix));


        Desc.fRotationPerSec = 20.f;
        Desc.fSpeedPerSec = 20.f;
        Desc.iModelNum = iModelNum;
        Desc.iModelListType = iObjectType;
        Desc.InitWorldMatrix = XMLoadFloat4x4(&WorldMatrix);

        
        if (wstrMyLayerName == L"Layer_Map_Object")
        {
            if (wstrMyPrototypeName == TEXT("Prototype_GameObject_Decorative_Object"))
            {
                iObjectType = CLevel_MapTool::DECORATIVE_OBJECT;
            }
            else if (wstrMyPrototypeName == TEXT("Prototype_GameObject_Static_Object"))
            {
                iObjectType = CLevel_MapTool::STATIC_OBJECT;
            }
            else if (wstrMyPrototypeName == TEXT("Prototype_GameObject_Dynamic_Object"))
            {
                iObjectType = CLevel_MapTool::DYNAMIC_OBJECT;
            }

        }
       

            
        switch (iObjectType)
        {
        case Client::CLevel_MapTool::DECORATIVE_OBJECT:
            wstrMyLayerName = L"Layer_Decorative_Object";
            break;
        case Client::CLevel_MapTool::STATIC_OBJECT:
            wstrMyLayerName = L"Layer_Static_Object";
            break;
        case Client::CLevel_MapTool::DYNAMIC_OBJECT:
            wstrMyLayerName = L"Layer_Dynamic_Object";
            break;
        case Client::CLevel_MapTool::OBJECT_TYPE_END:
            break;
        default:
            break;
        }
        

        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, wstrMyLayerName, wstrMyPrototypeName, &Desc);

    }

 

    fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(iObjectCount));
    for (_uint i = 0; i < iObjectCount; i++)
    {
        // ��ü���� //
        fin.read(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType));
        fin.read(reinterpret_cast<char*>(&iModelNum), sizeof(iModelNum));

        // ������Ÿ�� �̸� �б�
        ReadString(fin, strMyPrototypeName);
        wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

        // ���̾� �̸� �б�
        ReadString(fin, strMyLayerName);
        wstrMyLayerName = stringToWstring(strMyLayerName);


        // ���� ��� �б�
        _float4x4 WorldMatrix;
        fin.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix));


        Desc.fRotationPerSec = 20.f;
        Desc.fSpeedPerSec = 20.f;
        Desc.iModelNum = iModelNum;
        Desc.iModelListType = iObjectType;
        Desc.InitWorldMatrix = XMLoadFloat4x4(&WorldMatrix);


        if (wstrMyLayerName == L"Layer_Map_Object")
        {
            if (wstrMyPrototypeName == TEXT("Prototype_GameObject_Decorative_Object"))
            {
                iObjectType = CLevel_MapTool::DECORATIVE_OBJECT;
            }
            else if (wstrMyPrototypeName == TEXT("Prototype_GameObject_Static_Object"))
            {
                iObjectType = CLevel_MapTool::STATIC_OBJECT;
            }
            else if (wstrMyPrototypeName == TEXT("Prototype_GameObject_Dynamic_Object"))
            {
                iObjectType = CLevel_MapTool::DYNAMIC_OBJECT;
            }

        }



        switch (iObjectType)
        {
        case Client::CLevel_MapTool::DECORATIVE_OBJECT:
            wstrMyLayerName = L"Layer_Decorative_Object";
            break;
        case Client::CLevel_MapTool::STATIC_OBJECT:
            wstrMyLayerName = L"Layer_Static_Object";
            break;
        case Client::CLevel_MapTool::DYNAMIC_OBJECT:
            wstrMyLayerName = L"Layer_Dynamic_Object";
            break;
        case Client::CLevel_MapTool::OBJECT_TYPE_END:
            break;
        default:
            break;
        }


        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, wstrMyLayerName, wstrMyPrototypeName, &Desc);

    }



    fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(iObjectCount));
    for (_uint i = 0; i < iObjectCount; i++)
    {
        // ��ü���� //
        fin.read(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType));
        fin.read(reinterpret_cast<char*>(&iModelNum), sizeof(iModelNum));

        // ������Ÿ�� �̸� �б�
        ReadString(fin, strMyPrototypeName);
        wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

        // ���̾� �̸� �б�
        ReadString(fin, strMyLayerName);
        wstrMyLayerName = stringToWstring(strMyLayerName);


        // ���� ��� �б�
        _float4x4 WorldMatrix;
        fin.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix));


        Desc.fRotationPerSec = 20.f;
        Desc.fSpeedPerSec = 20.f;
        Desc.iModelNum = iModelNum;
        Desc.iModelListType = iObjectType;
        Desc.InitWorldMatrix = XMLoadFloat4x4(&WorldMatrix);


        if (wstrMyLayerName == L"Layer_Map_Object")
        {
            if (wstrMyPrototypeName == TEXT("Prototype_GameObject_Decorative_Object"))
            {
                iObjectType = CLevel_MapTool::DECORATIVE_OBJECT;
            }
            else if (wstrMyPrototypeName == TEXT("Prototype_GameObject_Static_Object"))
            {
                iObjectType = CLevel_MapTool::STATIC_OBJECT;
            }
            else if (wstrMyPrototypeName == TEXT("Prototype_GameObject_Dynamic_Object"))
            {
                iObjectType = CLevel_MapTool::DYNAMIC_OBJECT;
            }

        }



        switch (iObjectType)
        {
        case Client::CLevel_MapTool::DECORATIVE_OBJECT:
            wstrMyLayerName = L"Layer_Decorative_Object";
            break;
        case Client::CLevel_MapTool::STATIC_OBJECT:
            wstrMyLayerName = L"Layer_Static_Object";
            break;
        case Client::CLevel_MapTool::DYNAMIC_OBJECT:
            wstrMyLayerName = L"Layer_Dynamic_Object";
            break;
        case Client::CLevel_MapTool::OBJECT_TYPE_END:
            break;
        default:
            break;
        }


        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, wstrMyLayerName, wstrMyPrototypeName, &Desc);

    }








    fin.close();
    MSG_BOX(TEXT("LEVEL_MAPTOOL�� NonAnimObject ������ Load�߽��ϴ�."));

}


void CLevel_MapTool::Load_Navigation()
{
    ifstream fin;

    fin.open("../Bin/Navigation_Data.bin", ios::binary);
    if (!fin.is_open())    // ���� �����ٸ�
    {
        MSG_BOX(TEXT("�׺���̼� ������ �����ϴ�."));
        return;
    }


    // ������ �׺���̼� ?      // �ϴ� �ӽ÷� �ͷ��θ� �����ϰ� ���߿� �ٸ� �𵨵� �߰��ϸ� ���̾ ���� �׺� �ް��մ� ���ο����� ���̾� �������ҰͰ���.

    list<CGameObject*>& Terrians = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, TEXT("Layer_BackGround_Terrain"));

    vector<CCell*>& FreeCameraCells = m_pFreeCamera->Get_Navigation()->Get_Cells();

    for (auto& Terrain : Terrians)
    {
        CNavigation* pNavigation = Terrain->Get_Navigation();
        vector<CCell*>& Cells = pNavigation->Get_Cells();

        _uint iCellSize = {};

        fin.read(reinterpret_cast<_char*>(&iCellSize), sizeof(iCellSize));
        
        Cells.reserve(iCellSize);

        

        for (_uint i = 0; i < iCellSize; i++)
        {
            _int       iIndex = {};

            _float3     PointA = {};
            _float3     PointB = {};
            _float3     PointC = {};

            

            _int     NeighborsIndexAB = {};
            _int     NeighborsIndexBC = {};
            _int     NeighborsIndexCA = {};

            fin.read(reinterpret_cast<_char*>(&iIndex), sizeof(iIndex));

            fin.read(reinterpret_cast<_char*>(&PointA), sizeof(PointA));
            fin.read(reinterpret_cast<_char*>(&PointB), sizeof(PointB));
            fin.read(reinterpret_cast<_char*>(&PointC), sizeof(PointC));

            fin.read(reinterpret_cast<_char*>(&NeighborsIndexAB), sizeof(NeighborsIndexAB));
            fin.read(reinterpret_cast<_char*>(&NeighborsIndexBC), sizeof(NeighborsIndexBC));
            fin.read(reinterpret_cast<_char*>(&NeighborsIndexCA), sizeof(NeighborsIndexCA));

            _float3     vPoints[3] = { PointA, PointB, PointC };

            CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, iIndex);
            if (nullptr == pCell)
                return;


            pCell->Set_NeighborIndex(CCell::LINE_AB, NeighborsIndexAB);
            pCell->Set_NeighborIndex(CCell::LINE_BC, NeighborsIndexBC);
            pCell->Set_NeighborIndex(CCell::LINE_CA, NeighborsIndexCA);

            Cells.emplace_back(pCell);
            FreeCameraCells.emplace_back(pCell);
          
        }

    }


  



    fin.close();
    MSG_BOX(TEXT("LEVEL_MAPTOOL�� �׺���̼� ������ Load�߽��ϴ�."));
}


void CLevel_MapTool::Load_AnimObject_MapTool()
{
    ifstream fin;

    fin.open("../Bin/Anim_Model_Data_MapTool.bin", ios::binary);
    if (!fin.is_open())    // ���� �����ٸ�
    {
        MSG_BOX(TEXT("�������� Anim������ �����ϴ�\n���ð����� �޼����Դϴ�."));
        return;
    }

    _uint iObjectCount = {};

    _uint iObjectType = {};
    _uint iModelNum = {};


    string strMyPrototypeName = "";
    wstring wstrMyPrototypeName = L"";

    string strMyLayerName = "";
    wstring wstrMyLayerName = L"";

    fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(iObjectCount));


    Open_ObjectDialog();            // ������ Ready_Object�� ������Ÿ���� �������

    CGameObject::GAMEOBJECT_DESC Desc = {};


    // �� �÷��̾� 
    for (_uint i = 0; i < iObjectCount; i++)
    {
        // ��ü���� //
        fin.read(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType));
        fin.read(reinterpret_cast<char*>(&iModelNum), sizeof(iModelNum));

        // ������Ÿ�� �̸� �б�
        ReadString(fin, strMyPrototypeName);
        wstrMyPrototypeName = stringToWstring(strMyPrototypeName);

        // ���̾� �̸� �б�
        ReadString(fin, strMyLayerName);
        wstrMyLayerName = stringToWstring(strMyLayerName);


        // ���� ��� �б�
        _float4x4 WorldMatrix;
        fin.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(WorldMatrix));


        Desc.fRotationPerSec = 20.f;
        Desc.fSpeedPerSec = 20.f;
        Desc.iModelNum = iModelNum;
        Desc.iModelListType = iObjectType;
        Desc.InitWorldMatrix = XMLoadFloat4x4(&WorldMatrix);

        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, wstrMyLayerName, wstrMyPrototypeName, &Desc);
    }      



    fin.close();
    MSG_BOX(TEXT("LEVEL_MAPTOOL�� AnimObject ������ Load�߽��ϴ�."));

}






void CLevel_MapTool::ReadString(ifstream& fin, string& str)
{
    _uint length = 0;
    fin.read(reinterpret_cast<char*>(&length), sizeof(length));

    str.resize(length);
    fin.read(&str[0], length);
}


void CLevel_MapTool::ReadString(FILE* file, string& str)
{
    _uint length = 0;
    fread(&length, sizeof(length), 1, file);

    str.resize(length);
    fread(&str[0], length, 1, file);
}





CLevel_MapTool* CLevel_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_MapTool* pInstance = new CLevel_MapTool(pDevice,pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CLevel_MapTool"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_MapTool::Free()
{  

    // ����
    ImGui_ImplDX11_Shutdown();  // Dx ���� 
    ImGui_ImplWin32_Shutdown(); // Win32 ���� ������� X

    ImGui::DestroyContext(); // ������ �������� �ֱ�
    // ImGui ���� �����ؾ��ؿ�~~~

    Safe_Release(m_tPreViewObject.pTargetObject);
    Safe_Release(m_pFreeCamera);
    Safe_Release(m_pPickedObj);

    __super::Free();
}

