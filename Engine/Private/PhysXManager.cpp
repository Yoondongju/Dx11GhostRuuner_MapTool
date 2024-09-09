#include "../Public/PhysXManager.h"


CPhysXManager::CPhysXManager()
{

}

HRESULT CPhysXManager::Initialize()
{
	//if (FAILED(Ready_PhysX()))
	//	return E_FAIL;
	//
	//
	//m_pSimulationEventCallback = new MySimulationEventCallback();
	//m_pScene->setSimulationEventCallback(m_pSimulationEventCallback);
	

	return S_OK;
}



HRESULT CPhysXManager::Ready_PhysX()
{
	// Foundation ��ü ����
	static PxDefaultAllocator Allocator;
	static MyErrorCallback	  CallBackError;

	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, CallBackError);
	if (!m_pFoundation) {
		MSG_BOX(TEXT("PxCreateFoundation failed!"));
		return E_FAIL;
	}
	

	
	// PVD ����
	m_pPvd = PxCreatePvd(*m_pFoundation);
	if (!m_pPvd) {
		MSG_BOX(TEXT("PxCreatePvd failed!"));
		return E_FAIL;
	}
	
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	if (!transport) {
		MSG_BOX(TEXT("PxDefaultPvdSocketTransportCreate failed!"));
		return E_FAIL;
	}
	
	m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);



	// Physics ��ü ����
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
	if (!m_pPhysics) {
		MSG_BOX(TEXT("PxCreatePhysics  failed!"));
		return E_FAIL;
	}

	PxInitExtensions(*m_pPhysics, m_pPvd);


	// Scene ��ü ����
	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, 0.f, 0.0f);
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


	PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();


	if (pPvdClient)
	{
		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true); //��ü ����

		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true); // �浹 ����

		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true); // Scene ����
	}





	// Material ��ü ����
	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	if (!m_pMaterial) {
		MSG_BOX(TEXT("createMaterial failed!"));
		return E_FAIL;
	}



	MSG_BOX(TEXT("PhysX�� ���������� �ʱ�ȭ�Ǿ����ϴ�."));
	return S_OK;
}


void CPhysXManager::Update()
{
	// Static ��ü : ���� �ùķ��̼ǿ��� ���������ݴ´�				 (����,�� , �ǹ��� �������� �ʴ� ������ ��ü)
	// Dynamic ��ü : ���� �ùķ��̼ǿ� ��ġ�� �ӵ��� ������Ʈ�ȴ�.  (ĳ����, �������� ��ü���)  ������� ũ��

	// �� ������Ʈ
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


	if (m_pSimulationEventCallback)
	{
		delete m_pSimulationEventCallback;
		m_pSimulationEventCallback = nullptr;
	}

	if (m_pScene)
		m_pScene->release();

	if (m_pDispatcher)
		m_pDispatcher->release();

	if (m_pMaterial)
		m_pMaterial->release();

	if (m_pPhysics)
		m_pPhysics->release();

	if (m_pPvd)
	{
		m_pPvd->disconnect();  // PVD ���� ����
		m_pPvd->release();
	}

	if (m_pFoundation)
		m_pFoundation->release();
}
