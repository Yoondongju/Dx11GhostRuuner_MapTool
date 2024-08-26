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

	/* �� �ִϸ��̼��� ����ϴ� ���� ���� */
	_uint						m_iNumChannels = { 0 };
	/* ���� ������ ����*/
	/* Channel : �� (�ð��� ���� �� ���� ���� ����� �����Ѵ�) */

	vector<class CChannel*>		m_Channels;

	/* �� �ִϸ��̼��� �����ϱ����� �Ÿ��� ��ü �Ÿ�. */
	_double						m_Duration = {};

	/* �ִϸ��̼��� �ʴ� ����ӵ� */
	_double						m_SpeedPerSec = {};

	/* m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta */	 // �� �ִ��� Ʈ���������� ���� �𵨸��� ����վ��
	//_double						m_CurrentTrackPosition = {};	


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, vector<_uint>& KeyFrameIndices ,const class CModel* pModel);
	virtual void Free() override;
};

END
