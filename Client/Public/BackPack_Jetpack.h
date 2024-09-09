#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBackPack_Jetpack final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _uint*	 pParentState = { nullptr };
		const _float4x4* pSocketBoneMatrix = { nullptr };
	}BACKPACK_DESC;

private:
	CBackPack_Jetpack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackPack_Jetpack(const CBackPack_Jetpack& Prototype);
	virtual ~CBackPack_Jetpack() = default;


public:
	class CModel* Get_Model() override { return m_pModelCom; }



public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };



private:
	const _float4x4* m_pSocketMatrix = { nullptr };


private:
	HRESULT Ready_Components();

public:
	static CBackPack_Jetpack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END