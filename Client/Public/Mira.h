#pragma once


#include "Client_Defines.h"
#include "Monster.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CMira final : public CMonster
{
private:
	CMira(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMira(const CMira& Prototype);
	virtual ~CMira() = default;

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
	CModel* m_pModel = { nullptr };




private:
	HRESULT		Ready_Component();


public:
	static CMira* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END