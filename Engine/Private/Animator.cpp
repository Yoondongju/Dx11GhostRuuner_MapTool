#include "../Public/Animator.h"

CAnimator::CAnimator(ID3D11Device*  pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
	m_eComponentType = ANIMATOR;
}

CAnimator::CAnimator(const CAnimator& Prototype)
	: CComponent{ Prototype }
{
	m_eComponentType = ANIMATOR;
}

HRESULT CAnimator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimator::Initialize(void* pArg)
{
	return S_OK;
}

void CAnimator::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	if (m_pCurrentAnimationDesc->fDuration <= m_fAccTime && !m_isEnd)
	{
		++m_iCurrentFrame;

		if (m_iCurrentFrame >= m_pCurrentAnimationDesc->iEndFrame)
		{
			if (!m_pCurrentAnimationDesc->isLoop)
				m_isEnd = true;

			m_iCurrentFrame = m_pCurrentAnimationDesc->iStartFrame;
		}
		m_fAccTime = 0.f;
	}
}



HRESULT CAnimator::Change_Animation(const _wstring& strAnimationTag, _uint iStartFrame)
{
	if (nullptr != m_pCurrentAnimation)
	{
		Safe_Release(m_pCurrentAnimation);
	}

	m_pCurrentAnimation = Find_Animation(strAnimationTag);
	if (nullptr == m_pCurrentAnimation)
		return E_FAIL;

	auto	iter = m_AnimationDesc.find(strAnimationTag);
	m_pCurrentAnimationDesc = iter->second;
	Reset_Frame();

	Safe_AddRef(m_pCurrentAnimation);

	m_isEnd = false;

	return S_OK;
}

HRESULT CAnimator::Add_Animation(const _wstring& strAnimationTag, CTexture* pTexture, _uint iStartFrame, _uint iEndFrame, _float fDuration, _bool isLoop)
{
	if (Find_Animation(strAnimationTag) != nullptr)
		return E_FAIL;

	m_Animation.emplace(strAnimationTag, pTexture);

	ANIMATION_DESC* Desc = new ANIMATION_DESC;
	Desc->iStartFrame = iStartFrame;
	Desc->iEndFrame = iEndFrame;
	Desc->fDuration = fDuration;
	Desc->isLoop = isLoop;

	m_AnimationDesc.emplace(strAnimationTag, Desc);

	return S_OK;
}

CTexture* CAnimator::Find_Animation(const _wstring& strAnimationTag)
{
	auto	iter = m_Animation.find(strAnimationTag);
	if (iter == m_Animation.end())
		return nullptr;

	return iter->second;
}

void CAnimator::Reset_Frame()
{
	m_iCurrentFrame = m_pCurrentAnimationDesc->iStartFrame;
	m_fAccTime = 0.f;
	m_isEnd = false;
}

CAnimator* CAnimator::Create(ID3D11Device*  pDevice, ID3D11DeviceContext* pContext)
{
	CAnimator* pInstance = new CAnimator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CAnimator::Clone(void* pArg)
{
	CAnimator* pInstance = new CAnimator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimator::Free()
{
	__super::Free();

	Safe_Release(m_pCurrentAnimation);

	for (auto& Pair : m_Animation)
		Safe_Release(Pair.second);

	for (auto& Pair : m_AnimationDesc)
		Safe_Delete(Pair.second);

	m_Animation.clear();
}
