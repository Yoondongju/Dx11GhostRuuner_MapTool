#pragma once

#include "Base.h"


BEGIN(Engine)

class MyErrorCallback : public PxErrorCallback
{
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override
	{
		//MSG_BOX(TEXT("Phys �ʱ�ȭ ����"));
	}
};




class MySimulationEventCallback : public PxSimulationEventCallback
{
public:
    // �浹�� �߻��ϰų� ���� ���� �� ȣ��
    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 pairCount) override
    {
        for (PxU32 i = 0; i < pairCount; ++i)
        {
            const PxContactPair& pair = pairs[i];

            // �浹�� ���Ϳ� ��� ����
            PxActor* actor0 = pairHeader.actors[0];
            PxActor* actor1 = pairHeader.actors[1];

            // �浹 ���� �÷��� Ȯ��
            if (pair.flags.isSet(PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH))
            {
                // �浹�� ���� ���۵�
                printf("Contact started between Actor %p and Actor %p\n", actor0, actor1);
            }
            else if (pair.flags.isSet(PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH))
            {
                // �浹�� �����
                printf("Contact ended between Actor %p and Actor %p\n", actor0, actor1);
            }

            // �浹 ���� ���� ���
            for (PxU32 j = 0; j < pair.contactCount; ++j)
            {
                const PxU8& contact = pair.contactPoints[j];
               
            }
        }
    }

    // ������ �޼ҵ� ���� (�ʿ��� ���)
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
	PxFoundation*				m_pFoundation = nullptr;		// ����̽���������? physX ���� ��ü�� �갡 ���� �����Ǿ�� ��밡���ϴ�.

	PxPhysics*					m_pPhysics = nullptr;			// ���� �ùķ��̼��� �����ϴ� �� �ʿ��� ����� ����
	PxScene*					m_pScene = nullptr;				// ���� �ùķ��̼��� Ư�� ����� ��Ÿ����, ���� ��ü���� ��ȣ�ۿ��� ó���մϴ�. ���� �ùķ��̼��� ���¸� ������Ʈ�ϰ�, �ùķ��̼� ����� �������� ����
	PxMaterial*					m_pMaterial = nullptr;			// ���� ��ü�� ������ ������ �����ϴ� ������ ��Ÿ���ϴ�. ������, �ݹ߷� �� ������ ��ȣ�ۿ�

	PxDefaultCpuDispatcher*		m_pDispatcher = nullptr;		// ���� �ùķ��̼��� ��Ƽ�������� ���� ( �ϴ� ���� �ʿ���� )
	PxPvd*						m_pPvd = nullptr;


    // �浹 �ݹ��� ���� ��� ����
    MySimulationEventCallback* m_pSimulationEventCallback = nullptr;


private:
	HRESULT	Ready_PhysX();

public:
	static CPhysXManager* Create();
	virtual void Free() override;
};

END