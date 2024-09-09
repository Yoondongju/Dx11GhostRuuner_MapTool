#pragma once

#include "Base.h"


BEGIN(Engine)

class MyErrorCallback : public PxErrorCallback
{
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override
	{
		//MSG_BOX(TEXT("Phys 초기화 에러"));
	}
};




class MySimulationEventCallback : public PxSimulationEventCallback
{
public:
    // 충돌이 발생하거나 진행 중일 때 호출
    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 pairCount) override
    {
        for (PxU32 i = 0; i < pairCount; ++i)
        {
            const PxContactPair& pair = pairs[i];

            // 충돌한 액터와 모양 정보
            PxActor* actor0 = pairHeader.actors[0];
            PxActor* actor1 = pairHeader.actors[1];

            // 충돌 쌍의 플래그 확인
            if (pair.flags.isSet(PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH))
            {
                // 충돌이 새로 시작됨
                printf("Contact started between Actor %p and Actor %p\n", actor0, actor1);
            }
            else if (pair.flags.isSet(PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH))
            {
                // 충돌이 종료됨
                printf("Contact ended between Actor %p and Actor %p\n", actor0, actor1);
            }

            // 충돌 지점 정보 출력
            for (PxU32 j = 0; j < pair.contactCount; ++j)
            {
                const PxU8& contact = pair.contactPoints[j];
               
            }
        }
    }

    // 나머지 메소드 구현 (필요한 경우)
    void onTrigger(PxTriggerPair* pairs, PxU32 pairCount) override {}
    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {}
    void onWake(PxActor** actors, PxU32 count) override {}
    void onSleep(PxActor** actors, PxU32 count) override {}
    void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {}
};

END

BEGIN(Engine)

class CPhysXManager final : public CBase
{
private:
	CPhysXManager();
	virtual ~CPhysXManager() = default; 
	
public:
	PxPhysics*	Get_Physics() { return m_pPhysics; }
	PxScene*	Get_Scene() { return m_pScene; }
	PxMaterial* Get_Material() { return m_pMaterial; }






public:
	HRESULT Initialize();
	void Update();





private:
	PxFoundation*				m_pFoundation = nullptr;		// 디바이스같은느낌? physX 관렬 객체는 얘가 먼저 생성되어야 사용가능하다.

	PxPhysics*					m_pPhysics = nullptr;			// 물리 시뮬레이션을 수행하는 데 필요한 기능을 제공
	PxScene*					m_pScene = nullptr;				// 물리 시뮬레이션의 특정 장면을 나타내며, 물리 객체들의 상호작용을 처리합니다. 물리 시뮬레이션의 상태를 업데이트하고, 시뮬레이션 결과를 가져오는 역할
	PxMaterial*					m_pMaterial = nullptr;			// 물리 객체의 물리적 성질을 정의하는 재질을 나타냅니다. 마찰력, 반발력 등 물리적 상호작용

	PxDefaultCpuDispatcher*		m_pDispatcher = nullptr;		// 물리 시뮬레이션의 멀티스레딩을 관리 ( 일단 딱히 필요없음 )
	PxPvd*						m_pPvd = nullptr;


    // 충돌 콜백을 위한 멤버 변수
    MySimulationEventCallback* m_pSimulationEventCallback = nullptr;


private:
	HRESULT	Ready_PhysX();

public:
	static CPhysXManager* Create();
	virtual void Free() override;
};

END