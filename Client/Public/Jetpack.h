#pragma once


#include "Client_Defines.h"
#include "ContainerObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CJetpack final : public CContainerObject
{
public:
	enum PARTID{ PART_WEAPON , PART_BACKPACK , PART_END};

private:
	CJetpack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJetpack(const CJetpack& Prototype);
	virtual ~CJetpack() = default;

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
	HRESULT		Ready_Parts();


public:
	static CJetpack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END