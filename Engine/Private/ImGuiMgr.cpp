#include "../Public/ImGuiMgr.h"

CImGuiMgr::CImGuiMgr(LPDIRECT3DDEVICE9 pGraphic_Device)
    : m_pGraphic_Device{pGraphic_Device}
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CImGuiMgr::Initialize(const ENGINE_DESC& EngineDesc)
{
	
	// 생성부분
	m_ImGuiContext = ImGui::CreateContext();          // Create 무조건 먼저 해줘야함 , 얘가 순서가 먼저여야함
	ImGui::StyleColorsDark();        // Style -> 색상

	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig font_cfg;
	font_cfg.OversampleH = 2;
	font_cfg.OversampleV = 2;
	font_cfg.PixelSnapH = true;

	// 폰트 파일 경로를 적절하게 변경하세요
	io.Fonts->AddFontFromFileTTF("C://Windows//Fonts//gulim.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean());


	if (!ImGui_ImplWin32_Init(EngineDesc.hWnd))  // win32 관련된 초기화
	{
		MSG_BOX(L"IMGUI WIN32 초기화 실패");
		return E_FAIL;
	}

	if (!ImGui_ImplDX9_Init(m_pGraphic_Device))  // Dx 관련된 초기화
	{
		MSG_BOX(L"IMGUI DX9 초기화 실패");
		return E_FAIL;
	}
	

	ImGui::SetCurrentContext(m_ImGuiContext);

    return S_OK;
}

void CImGuiMgr::Render()
{	
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();


	ImGui::Begin("Example Window");
	if (ImGui::Button("Click Me"))
	{
		// 버튼이 클릭되었을 때 처리
		printf("Button was clicked!\n");
	}
	ImGui::Text("Move the slider:");
	float shininess = 10.f;
	ImGui::SliderFloat("Shininess", &shininess, 1.0f, 50.0f);
	ImGui::End();
	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

CImGuiMgr* CImGuiMgr::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const ENGINE_DESC& EngineDesc)
{
	CImGuiMgr* pInstance = new CImGuiMgr(pGraphic_Device);

	if (FAILED(pInstance->Initialize(EngineDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : ImGuiMgr"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGuiMgr::Free()
{
	// 해제
	ImGui_ImplDX9_Shutdown();  // Dx 해제 
	ImGui_ImplWin32_Shutdown(); // Win32 해제 순서상관 X

	ImGui::DestroyContext(); // 무조건 마지막에 넣기
	// ImGui 먼저 삭제해야해요~~~


	__super::Free();

	Safe_Release(m_pGraphic_Device);

}
