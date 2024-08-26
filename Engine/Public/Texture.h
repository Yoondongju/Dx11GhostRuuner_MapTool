#pragma once
#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TYPE { TYPE_TEXTURE2D/* : LPDIRECT3DTEXTURE9*/, TYPE_TEXTURECUBE/* : LPDIRECT3DCUBETEXTURE9*/, TYPE_END };

private:
	CTexture(ID3D11Device*  pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

public:
	ID3D11ShaderResourceView* Get_ShaderResource(_uint iNum)
	{
		if (iNum >= m_iNumTextures)
			assert(nullptr);

		return m_SRVs[iNum];
	}

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	

private:
	_uint											m_iNumTextures = { 0 };
	vector<ID3D11ShaderResourceView*>				m_SRVs;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual CComponent* Clone(void *pArg) override;
	virtual void Free() override;
};

END
