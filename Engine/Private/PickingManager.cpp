#include "../Public/PickingManager.h"

#include "../Public/Layer.h"
#include "../Public/VIBuffer.h"
#include "../Public/Transform.h"

#include "../Public/GameObject.h"
#include "../Public/GameInstance.h"

CPickingManager::CPickingManager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice{ pDevice}
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPickingManager::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	_uint			iNumViewports = { 1 };

	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	D3D11_TEXTURE2D_DESC	TextureDesc{};
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = m_iViewportWidth = ViewportDesc.Width;
	TextureDesc.Height = m_iViewportHeight = ViewportDesc.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = 0;

	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pPickDepthTexture)))
		return E_FAIL;

	return S_OK;
}

_bool CPickingManager::Picking(_float3* pPickPos)
{
	if (FAILED(m_pGameInstance->Copy_RenderTarget(TEXT("Target_PickDepth"), m_pPickDepthTexture)))
		return false;

	POINT			ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	_uint		iPixelIndex = ptMouse.y * m_iViewportWidth + ptMouse.x;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pPickDepthTexture, 0, D3D11_MAP_READ, 0, &SubResource);

	_float		fProjZ = ((_float4*)SubResource.pData)[iPixelIndex].x;
	_bool		isPicked = static_cast<_bool>(((_float4*)SubResource.pData)[iPixelIndex].w);

	m_pContext->Unmap(m_pPickDepthTexture, 0);

	if (false == isPicked)
		return false;

	_float4 vProjPos;

	/* 투영공간상의 위치. */
	vProjPos.x = ptMouse.x / (m_iViewportWidth * 0.5f) - 1.f;
	vProjPos.y = ptMouse.y / -(m_iViewportHeight * 0.5f) + 1.f;
	vProjPos.z = fProjZ;
	vProjPos.w = 1.f;

	/* 뷰공간상의 위치. */
	_vector	vViewPos = XMVector3TransformCoord(XMLoadFloat4(&vProjPos), m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ));

	_vector vWorldPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW));

	XMStoreFloat3(pPickPos, vWorldPos);

	return true;
}

_vector CPickingManager::Compute_Height(_fvector vWorldPos, _fmatrix ViewMatrix, _fmatrix ProjMatrix)
{
	if (FAILED(m_pGameInstance->Copy_RenderTarget(TEXT("Target_Height"), m_pPickDepthTexture)))
		return XMVectorZero();

	_vector		vTargetProjPos = XMVector3TransformCoord(vWorldPos, ViewMatrix);
	vTargetProjPos = XMVector3TransformCoord(vTargetProjPos, ProjMatrix);

	/*-1 -> 0
	1 -> viewport.width

	1 -> 0
	-1 -> Viewport.height*/

	_float2		vTargetPos = _float2(XMVectorGetX(vTargetProjPos) * m_iViewportWidth * 0.5f + m_iViewportWidth * 0.5f,
		XMVectorGetY(vTargetProjPos) * m_iViewportHeight * -0.5f + m_iViewportHeight * 0.5f);


	_uint		iPixelIndex = _uint(vTargetPos.y) * m_iViewportWidth + _uint(vTargetPos.x);

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pPickDepthTexture, 0, D3D11_MAP_READ, 0, &SubResource);

	_float		fProjZ = ((_float4*)SubResource.pData)[iPixelIndex].x;
	_bool		isPicked = static_cast<_bool>(((_float4*)SubResource.pData)[iPixelIndex].w);

	m_pContext->Unmap(m_pPickDepthTexture, 0);

	if (false == isPicked)
		return XMVectorZero();

	_float4 vProjPos;

	/* 투영공간상의 위치. */
	vProjPos.x = vTargetPos.x / (m_iViewportWidth * 0.5f) - 1.f;
	vProjPos.y = vTargetPos.y / -(m_iViewportHeight * 0.5f) + 1.f;
	vProjPos.z = fProjZ;
	vProjPos.w = 1.f;

	/* 뷰공간상의 위치. */
	_vector	vViewPos = XMVector3TransformCoord(XMLoadFloat4(&vProjPos), XMMatrixInverse(nullptr, ProjMatrix));

	_vector vResultPos = XMVector3TransformCoord(vViewPos, XMMatrixInverse(nullptr, ViewMatrix));

	return vResultPos;
}


_float3 CPickingManager::Get_PickingObject_CenterPos(class CLayer* pLayer)
{
	// 1. 마우스의 스크린 좌표를 얻어온다
	// 2. 이 마우스의 월드 좌표를 구해야한다.
	// 3. 그러려면 거꾸로 가야해    투영 -> 뷰 - > 월드

	//D3DVIEWPORT9 ViewPort;
	//m_pDevice->GetViewport(&ViewPort);
	//
	//_float4x4 MatProjection;
	//m_pDevice->GetTransform(D3DTS_PROJECTION, &MatProjection);
	//
	//
	//POINT		ptMouse = {};
	//GetCursorPos(&ptMouse);
	//ScreenToClient(m_hWnd, &ptMouse);
	//
	//
	//
	//
	////(2.f *ptMouse.x) / ViewPort.Width  == 0 ~ 2 까지 정규화시키고 -1 빼주면 투영스페이스의 -1 ~ 1 범위나옴
	//
	//m_vOriginal = _float3{ 0.f,0.f,0.f }; // 카메라의 eye 위치
	//
	//m_vDirection.x = (((2.f * ptMouse.x) / ViewPort.Width) - 1.f) / MatProjection._11;
	//m_vDirection.y = (((-2.f * ptMouse.y) / ViewPort.Height) + 1.f) / MatProjection._22;
	//m_vDirection.z = 1.f;
	//
	//_float4x4 matView, matInvView;
	//m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	//
	//
	//D3DXMatrixInverse(&matInvView, nullptr, &matView);
	//
	////////// 여까지하면 내 마우스 좌표가 View 스페이스 변환까지햇다
	//// 이제 월드변환하자
	//
	//
	//D3DXVec3TransformCoord(&m_vOriginal, &m_vOriginal, &matInvView);
	//D3DXVec3TransformNormal(&m_vDirection, &m_vDirection, &matInvView);
	//// 뷰 역행렬 곱해서 월드좌표로 변환시켯다
	//
	//
	//
	//
	//_uint iCount = pLayer->Get_GamObjectSize();
	//
	//for (_uint i = 0; i < iCount; i++)
	//{	
	//	CComponent* pComponent = pLayer->Find_Component(TEXT("Com_VIBuffer"), i);
	//	CComponent* pComponent_Transfrom = pLayer->Find_Component(TEXT("Com_Transform"), i);
	//
	//
	//	CVIBuffer* pModelVIBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	//	CTransform* pTransfrom = dynamic_cast<CTransform*>(pComponent_Transfrom);
	//
	//
	//	
	//	_float4x4 InvWolrdMat = pTransfrom->Get_WorldMatrix_Inverse();
	//
	//	D3DXVec3TransformCoord(&m_vOriginal, &m_vOriginal, &InvWolrdMat);
	//	D3DXVec3TransformNormal(&m_vDirection, &m_vDirection, &InvWolrdMat);
	//	D3DXVec3Normalize(&m_vDirection, &m_vDirection);
	//
	//
	//	LPDIRECT3DVERTEXBUFFER9 pVB = pModelVIBuffer->Get_VB();
	//
	//	VTXPOSTEX* pVertices = { nullptr };
	//	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	//	_uint iX = pModelVIBuffer->m_iNumVerticesX;
	//	_uint iZ = pModelVIBuffer->m_iNumVerticesZ;
	//
	//	pVB->Lock(0, 0, (void**)&pVertices, 0);
	//
	//
	//	float		fU, fV, fDist;
	//	for (size_t i = 0; i < iZ-1; i++)
	//	{
	//		for (size_t j = 0; j < iX-1; j++)
	//		{
	//			_uint		iIndex = i * iX + j;
	//			_uint		iIndices[4] = {
	//				iIndex + iX,
	//				iIndex + iX + 1,
	//				iIndex + 1 ,
	//				iIndex,
	//			};
	//			//pVertices[iIndices[0]]  , pVertices[iIndices[1]] , pVertices[iIndices[2]]
	//			if (D3DXIntersectTri(&pVertices[iIndices[0]].vPosition,
	//				&pVertices[iIndices[1]].vPosition,
	//				&pVertices[iIndices[2]].vPosition,
	//				&m_vOriginal,
	//				&m_vDirection,
	//				&fU,
	//				&fV,
	//				&fDist))		// 왼상단 삼각형
	//			{	
	//				_float fX = (pVertices[iIndices[1]].vPosition.x - pVertices[iIndices[0]].vPosition.x) * 0.5f + j;
	//				_float fY = (pVertices[iIndices[1]].vPosition.y - pVertices[iIndices[2]].vPosition.y) * 0.5f;
	//				_float fZ = (pVertices[iIndices[0]].vPosition.z - pVertices[iIndices[2]].vPosition.z) * 0.5f + i;
	//
	//				
 	//				return _float3(fX, fY, fZ);
	//			}
	//
	//			else if (D3DXIntersectTri(&pVertices[iIndices[0]].vPosition,
	//				&pVertices[iIndices[2]].vPosition,
	//				&pVertices[iIndices[3]].vPosition,
	//				&m_vOriginal,
	//				&m_vDirection,
	//				&fU,
	//				&fV,
	//				&fDist))	   // 우하단 삼각형
	//			{
	//				_float fX = (pVertices[iIndices[2]].vPosition.x - pVertices[iIndices[3]].vPosition.x) * 0.5f + j;
	//				_float fY = (pVertices[iIndices[0]].vPosition.y - pVertices[iIndices[3]].vPosition.y) * 0.5f;
	//				_float fZ = (pVertices[iIndices[0]].vPosition.z - pVertices[iIndices[2]].vPosition.z) * 0.5f + i;
	//
	//				return _float3(fX, fY, fZ);
	//			}
	//		}
	//	}
	//	pVB->Unlock();
	//}

	return _float3(0.f, 0.f, 0.f);
}

_float3 CPickingManager::Get_PickingObject_JustPos(CLayer* pLayer)
{
	//D3DVIEWPORT9 ViewPort;
	//m_pDevice->GetViewport(&ViewPort);
	//
	//_float4x4 MatProjection;
	//m_pDevice->GetTransform(D3DTS_PROJECTION, &MatProjection);
	//
	//
	//POINT		ptMouse = {};
	//GetCursorPos(&ptMouse);
	//ScreenToClient(m_hWnd, &ptMouse);
	//
	//
	//
	//
	////(2.f *ptMouse.x) / ViewPort.Width  == 0 ~ 2 까지 정규화시키고 -1 빼주면 투영스페이스의 -1 ~ 1 범위나옴
	//
	//m_vOriginal = _float3{ 0.f,0.f,0.f }; // 카메라의 eye 위치
	//
	//m_vDirection.x = (((2.f * ptMouse.x) / ViewPort.Width) - 1.f) / MatProjection._11;
	//m_vDirection.y = (((-2.f * ptMouse.y) / ViewPort.Height) + 1.f) / MatProjection._22;
	//m_vDirection.z = 1.f;
	//
	//_float4x4 matView, matInvView;
	//m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	//
	//
	//D3DXMatrixInverse(&matInvView, nullptr, &matView);
	//
	////////// 여까지하면 내 마우스 좌표가 View 스페이스 변환까지햇다
	//// 이제 월드변환하자
	//
	//
	//D3DXVec3TransformCoord(&m_vOriginal, &m_vOriginal, &matInvView);
	//D3DXVec3TransformNormal(&m_vDirection, &m_vDirection, &matInvView);
	//// 뷰 역행렬 곱해서 월드좌표로 변환시켯다
	//
	//
	//
	//
	//_uint iCount = pLayer->Get_GamObjectSize();
	//
	//for (_uint i = 0; i < iCount; i++)
	//{
	//	CComponent* pComponent = pLayer->Find_Component(TEXT("Com_VIBuffer"), i);
	//	CComponent* pComponent_Transfrom = pLayer->Find_Component(TEXT("Com_Transform"), i);
	//
	//
	//	CVIBuffer* pModelVIBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	//	CTransform* pTransfrom = dynamic_cast<CTransform*>(pComponent_Transfrom);
	//
	//
	//
	//	_float4x4 InvWolrdMat = pTransfrom->Get_WorldMatrix_Inverse();
	//
	//	D3DXVec3TransformCoord(&m_vOriginal, &m_vOriginal, &InvWolrdMat);
	//	D3DXVec3TransformNormal(&m_vDirection, &m_vDirection, &InvWolrdMat);
	//	D3DXVec3Normalize(&m_vDirection, &m_vDirection);
	//
	//
	//	LPDIRECT3DVERTEXBUFFER9 pVB = pModelVIBuffer->Get_VB();
	//
	//	VTXPOSTEX* pVertices = { nullptr };
	//	/* 정점버퍼의 내가 지정한 위치의 메모리 주소를 얻어올 수 있다. */
	//	_uint iX = pModelVIBuffer->m_iNumVerticesX;
	//	_uint iZ = pModelVIBuffer->m_iNumVerticesZ;
	//
	//	pVB->Lock(0, 0, (void**)&pVertices, 0);
	//
	//
	//	float		fU, fV, fDist;
	//	for (size_t i = 0; i < iZ - 1; i++)
	//	{
	//		for (size_t j = 0; j < iX - 1; j++)
	//		{
	//			_uint		iIndex = i * iX + j;
	//			_uint		iIndices[4] = {
	//				iIndex + iX,
	//				iIndex + iX + 1,
	//				iIndex + 1 ,
	//				iIndex,
	//			};
	//			//pVertices[iIndices[0]]  , pVertices[iIndices[1]] , pVertices[iIndices[2]]
	//			if (D3DXIntersectTri(&pVertices[iIndices[0]].vPosition,
	//				&pVertices[iIndices[1]].vPosition,
	//				&pVertices[iIndices[2]].vPosition,
	//				&m_vOriginal,
	//				&m_vDirection,
	//				&fU,
	//				&fV,
	//				&fDist))		// 왼상단 삼각형
	//			{
	//				int a = 1;
	//
	//				return _float3(m_vOriginal + (m_vDirection * fDist));
	//			}
	//
	//			else if (D3DXIntersectTri(&pVertices[iIndices[0]].vPosition,
	//				&pVertices[iIndices[2]].vPosition,
	//				&pVertices[iIndices[3]].vPosition,
	//				&m_vOriginal,
	//				&m_vDirection,
	//				&fU,
	//				&fV,
	//				&fDist))	   // 우하단 삼각형
	//			{
	//				int a = 1;
	//
	//
	//				return _float3(m_vOriginal + (m_vDirection * fDist));
	//			}
	//		}
	//	}
	//	pVB->Unlock();
	//}

	return _float3(0.f, 0.f, 0.f);
}

_float3 CPickingManager::Get_PickingBackGround_JustPos(_float fTargetY)
{
	D3D11_VIEWPORT ViewPort = {};
	_uint		   iViewPortNum = 1;
	m_pContext->RSGetViewports(&iViewPortNum,&ViewPort);
	

	_matrix InversProjectionMatrix = {};
	InversProjectionMatrix = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ);
	
	
	POINT		ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);
	

	_float3		vMousePos{};
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (ViewPort.Height * -0.5f) + 1.f;
	vMousePos.z = 0.f; /* Near평면을 클릭한거야!! */


	_vector MousePosTransToView = XMVector3TransformCoord(XMLoadFloat3(&vMousePos), InversProjectionMatrix);

	m_vOriginal = _float3{ 0.f,0.f,0.f}; // 카메라의 eye 위치

	XMStoreFloat3(&m_vDirection , MousePosTransToView - XMLoadFloat3(&m_vOriginal));
	

	_matrix InversViewMatrix = {};
	InversViewMatrix = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW);
	

	//////// 여까지하면 내 마우스 좌표가 View 스페이스 변환까지햇다
	// 이제 월드변환하자
	

	XMStoreFloat3(&m_vOriginal, XMVector3TransformCoord(XMLoadFloat3(&m_vOriginal), InversViewMatrix));	
	XMStoreFloat3(&m_vDirection, XMVector3TransformNormal(XMLoadFloat3(&m_vDirection), InversViewMatrix));

	XMStoreFloat3(&m_vDirection, XMVector3Normalize(XMLoadFloat3(&m_vDirection)));
	
	
	
	// m_vDirection.y * t + m_vOriginal.y == fTargetY				// y가 fTargetY일때의 t값은?
	float t = (fTargetY -m_vOriginal.y) / m_vDirection.y;			// y 위치가 평면 0 일때의 t값이거든..


	_vector vIntersection = XMLoadFloat3(&m_vOriginal) + (t * XMLoadFloat3(&m_vDirection));
	
	// 뷰 역행렬 곱해서 월드좌표로 변환시켯다

	_float3 vResult = {};
	XMStoreFloat3(&vResult, vIntersection);

	return vResult;
}

void CPickingManager::Update()
{
	D3D11_VIEWPORT ViewPort = {};
	_uint		   iViewPortNum = 1;
	m_pContext->RSGetViewports(&iViewPortNum, &ViewPort);


	_matrix InversProjectionMatrix = {};
	InversProjectionMatrix = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ);


	POINT		ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);


	_float3		vMousePos{};
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (ViewPort.Height * -0.5f) + 1.f;
	vMousePos.z = 0.f; /* Near평면을 클릭한거야!! */


	_vector MousePosTransToView = XMVector3TransformCoord(XMLoadFloat3(&vMousePos), InversProjectionMatrix);

	m_vOriginal = _float3{ 0.f,0.f,0.f }; // 카메라의 eye 위치
	XMStoreFloat3(&m_vDirection, MousePosTransToView - XMLoadFloat3(&m_vOriginal));


	_matrix InversViewMatrix = {};
	InversViewMatrix = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW);


	//////// 여까지하면 내 마우스 좌표가 View 스페이스 변환까지햇다
	// 이제 월드변환하자


	XMStoreFloat3(&m_vOriginal, XMVector3TransformCoord(XMLoadFloat3(&m_vOriginal), InversViewMatrix));
	XMStoreFloat3(&m_vDirection, XMVector3TransformNormal(XMLoadFloat3(&m_vDirection), InversViewMatrix));

	XMStoreFloat3(&m_vDirection, XMVector3Normalize(XMLoadFloat3(&m_vDirection)));

}

void CPickingManager::Transform_ToLocalSpace(const _float4x4& WorldMatrix)
{	
	_matrix		WorldMatrixInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMatrix));
	
	XMStoreFloat3(&m_vRayPos_InLocalSpace , XMVector3TransformCoord(XMLoadFloat3(&m_vOriginal), WorldMatrixInverse));

	XMStoreFloat3(&m_vRayDir_InLocalSpace, XMVector3TransformNormal(XMLoadFloat3(&m_vDirection), WorldMatrixInverse));
	
	XMStoreFloat3(&m_vRayDir_InLocalSpace, XMVector3Normalize(XMLoadFloat3(&m_vRayDir_InLocalSpace)));
}

_bool CPickingManager::isPicked_InWorldSpace(const BoundingBox& MeshBoundingBox, _float3* pOut)
{
	return false;
}

_bool CPickingManager::isPicked_InLocalSpace(const BoundingBox& MeshBoundingBox, _float3* pOut)
{
	_float distance;
	if (MeshBoundingBox.Intersects(XMLoadFloat3(&m_vRayPos_InLocalSpace), XMLoadFloat3(&m_vRayDir_InLocalSpace), distance))
	{
		XMVECTOR intersectionPoint = XMLoadFloat3(&m_vRayPos_InLocalSpace) + XMLoadFloat3(&m_vRayDir_InLocalSpace) * distance;
		XMStoreFloat3(pOut, intersectionPoint);
		return true;
	}

	return false;
}

_bool CPickingManager::isPicked_InLocalSpace(const _vector& vPointA, const _vector& vPointB, const _vector& vPointC, _vector* pOut)
{
	float fDist = 0.0f;
	_bool hit = DirectX::TriangleTests::Intersects(XMLoadFloat3(&m_vRayPos_InLocalSpace), XMLoadFloat3(&m_vRayDir_InLocalSpace), vPointA, vPointB, vPointC, fDist);

	if (hit)
	{
		*pOut = XMLoadFloat3(&m_vRayPos_InLocalSpace) + XMLoadFloat3(&m_vRayDir_InLocalSpace) * fDist;
		return true;
	}
	return false;
}


CPickingManager* CPickingManager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext ,HWND hWnd)
{
	CPickingManager* pInstance = new CPickingManager(pDevice,pContext);

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CPickingManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPickingManager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pPickDepthTexture);
}
