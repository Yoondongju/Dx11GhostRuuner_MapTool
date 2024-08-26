#include "../Public/PhysXManager.h"


CPhysXManager::CPhysXManager()
{

}

HRESULT CPhysXManager::Initialize()
{
	if (FAILED(Ready_PhysX()))
		return E_FAIL;


	

	return S_OK;
}



HRESULT CPhysXManager::Ready_PhysX()
{
	// Foundation 객체 생성
	static PxDefaultAllocator Allocator;
	static MyErrorCallback	  CallBackError;

	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, CallBackError);
	if (!m_pFoundation) {
		MSG_BOX(TEXT("PxCreateFoundation failed!"));
		return E_FAIL;
	}
	

	// Physics 객체 생성
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true);
	if (!m_pPhysics) {
		MSG_BOX(TEXT("PxCreatePhysics  failed!"));
		return E_FAIL;
	}

	// PVD 설정
	//m_pPvd = PxCreatePvd(*m_pFoundation);
	//if (!m_pPvd) {
	//	MSG_BOX(TEXT("PxCreatePvd failed!"));
	//	return E_FAIL;
	//}
	//
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//if (!transport) {
	//	MSG_BOX(TEXT("PxDefaultPvdSocketTransportCreate failed!"));
	//	return E_FAIL;
	//}
	//
	//m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);



	// Scene 객체 생성
	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	if (!m_pDispatcher) {
		MSG_BOX(TEXT("PxDefaultCpuDispatcherCreate failed!"));
		return E_FAIL;
	}
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_pScene = m_pPhysics->createScene(sceneDesc);
	if (!m_pScene) {
		MSG_BOX(TEXT("createScene failed!"));
		return E_FAIL;
	}


	// Material 객체 생성
	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	if (!m_pMaterial) {
		MSG_BOX(TEXT("createMaterial failed!"));
		return E_FAIL;
	}



	MSG_BOX(TEXT("PhysX가 성공적으로 초기화되었습니다."));
	return S_OK;
}


void CPhysXManager::Update()
{
	// Static 물체 : 물리 시뮬레이션에서 움직이이잖는다				 (지면,벽 , 건물등 움직이지 않는 고정된 물체)
	// Dynamic 물체 : 물리 시뮬레이션에 위치나 속도가 업데이트된다.  (캐릭터, 떨어지는 물체등등)  연산부하 크다

	// 씬 업데이트
	if (m_pScene)
		m_pScene->simulate(1.0f / 60.0f); // 60 FPS
	if (m_pScene)
		m_pScene->fetchResults(true);
	
}


CPhysXManager* CPhysXManager::Create()
{
	CPhysXManager* pInstance = new CPhysXManager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPhysXManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysXManager::Free()
{
	__super::Free();


	m_pPvd->release();
	m_pFoundation->release();
	m_pPhysics->release();
	m_pScene->release();
	m_pMaterial->release();
	m_pDispatcher->release();
}
