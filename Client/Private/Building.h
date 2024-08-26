#pragma once


#include "Client_Defines.h"
#include "GameObject.h"




BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CBuilding final : public CGameObject
{
private:
	CBuilding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBuilding(const CBuilding& Prototype);
	virtual ~CBuilding() = default;


public:
	class CModel* Get_Model() override { return m_pModel; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CModel*  m_pModel = { nullptr };


	PxRigidStatic*	m_pRigidStatic = { nullptr };  // ���� ��ü�� ��Ÿ���� PhysX ��ü
	PxTransform		m_Transform = {};     // ������ ��ȯ ���� (��ġ�� ȸ��)

private:
	HRESULT		Ready_Component();
	

public:
	static CBuilding* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END