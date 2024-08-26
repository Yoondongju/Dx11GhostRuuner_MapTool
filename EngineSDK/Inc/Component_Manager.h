#pragma once

#include "VIBuffer_Terrain.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_GridRect.h"
#include "VIBuffer_Cube.h"

#include "Model.h"
 

#include "Transform.h"
#include "Texture.h"
#include "Shader.h"

#include "Animator.h"
#include "Fsm.h"
#include "Navigation.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;


public:
	class CComponent* Get_Prototype(_uint iNumLevelIndex, const _wstring& strPrototypeTag)
	{
		return Find_Prototype(iNumLevelIndex, strPrototypeTag);
	}


public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iNumLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);

	class CComponent* Clone_Component(_uint iNumLevelIndex, const _wstring& strPrototypeTag, void* pArg);


private:
	map<const _wstring, class CComponent*>*		m_pPrototype = { nullptr };
	_uint										m_iNumLevels = { 0 };

private:
	class CComponent* Find_Prototype(_uint iNumLevelIndex, const _wstring& strPrototypeTag);
	
public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END

