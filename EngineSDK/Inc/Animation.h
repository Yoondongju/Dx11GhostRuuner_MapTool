#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;
	

public:
	vector<class CChannel*>& Get_Channels() { return m_Channels; }

	_uint					 Get_NumChannel() { return m_iNumChannels; }
	_double					 Get_Duration() { return m_Duration; }
	_double					 Get_SpeedPerSec() { return m_SpeedPerSec; }
	
	_char*					 Get_Name() { return m_szName; }

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, vector<_uint>& KeyFrameIndices , const class CModel* pModel);

	_bool  Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition,  vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _float fTimeDelta, _bool isTransitioning , CAnimation* pNextAnimation = nullptr );



private:
	_char						m_szName[MAX_PATH] = {};

	/* 이 애니메이션이 사용하는 뼈의 갯수 */
	_uint						m_iNumChannels = { 0 };
	/* 실제 뼈들의 정보*/
	/* Channel : 뼈 (시간에 따른 이 뼈의 상태 행렬을 보관한다) */

	vector<class CChannel*>		m_Channels;

	/* 이 애니메이션을 구동하기위해 거리는 전체 거리. */
	_double						m_Duration = {};

	/* 애니메이션의 초당 재생속도 */
	_double						m_SpeedPerSec = {};

	/* m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta */	 // 내 애니의 트랙포지션은 이제 모델마다 들고잇어용
	//_double						m_CurrentTrackPosition = {};	


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, vector<_uint>& KeyFrameIndices ,const class CModel* pModel);
	virtual void Free() override;
};

END
