#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_Player final : public CPartObject
{
public:
	enum WEAPON_TYPE { KATANA, SHURIKEN, WEAPON_TYPE_END};

	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketBoneMatrix = { nullptr };
	}WEAPON_DESC;

private:
	CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Player(const CWeapon_Player& Prototype);
	virtual ~CWeapon_Player() = default;


public:
	class CModel* Get_Model() override { return m_pModelCom[m_eCurType]; }
	class CModel* Get_TypeModel(WEAPON_TYPE eType)  { return m_pModelCom[eType]; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel*  m_pModelCom[WEAPON_TYPE::WEAPON_TYPE_END] = {nullptr};

	WEAPON_TYPE		m_eCurType = { WEAPON_TYPE::KATANA };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	

private:
	HRESULT Ready_Components();

public:
	static CWeapon_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END