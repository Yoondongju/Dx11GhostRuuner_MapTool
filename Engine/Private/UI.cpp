#include "../Public/UI.h"

#include "../Public/GameInstance.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice,pContext }
{
}

CUI::CUI(const CGameObject& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

	/* 유아이의 초기 위치와 사이즈를 설정한다. */
	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	_uint		iNumView = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumView, &ViewportDesc);

	/* m_ViewMatrix : XMFLOAT4X4 */
	/* XMMatrixIdentity함수의 리턴값 XMMATRIX */
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_fViewWidth = ViewportDesc.Width;
	m_fViewHeight = ViewportDesc.Height;

	// XMVector3Equal(XMLoadFloat3(&m_vTmp), XMLoadFloat3(&m_vTmp));	

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CUI::Priority_Update(_float fTimeDelta)
{
}

void CUI::Update(_float fTimeDelta)
{
}

void CUI::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드 행렬을 만들어 준다. */

	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - m_fViewWidth * 0.5f, -m_fY + m_fViewHeight * 0.5f, 0.f, 1.f));
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::MouseOnCheck()
{
	POINT vMousePos = m_pGameInstance->Get_MosePos();	
	
	RECT tUIRect;
	SetRect(&tUIRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f,
		m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	if (vMousePos.x > tUIRect.left
		&& vMousePos.x < tUIRect.right
		&& vMousePos.y > tUIRect.top
		&& vMousePos.y < tUIRect.bottom)
	{
		m_bMouseOn = true;
	}
	else
		m_bMouseOn = false;

}

void CUI::MouseLBtnDown()
{
}

void CUI::MouseLBtnUp()
{
}

void CUI::MouseClick()
{
}

void CUI::MouseOn()
{
}

void CUI::MouseOut()
{
}


void CUI::Free()
{
	__super::Free();
}
