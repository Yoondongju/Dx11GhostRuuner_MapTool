#pragma once

#include "Base.h"

BEGIN(Engine)

/* �ִԿ��ó��� ������ ���� ����ϴ� �� �� �ϳ� */
/* �ð��� ���� ���� ������ ������ �ִ´�. �ʱ� ������ ��� �ð��� ���� ���°����� �̸� �����صд�. */
/* ���� ��Ÿ�Ӱ��� �� ä�ο��� ���� �ִϸ��̼��� ��� ��ġ�� �������ָ� �����ġ�� �´� �������(m_TransformatoinMatrix )�� �����. */

/* KEYFRAME : Ư�� �����ġ���� �� ���� ���ؾ��� ����(���)�� ����(������)�Ͽ� ǥ���� ����ü.*/

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	_char* Get_Name() { return m_szName; }
	_uint  Get_BoneIndex() { return m_iBoneIndex; }
	_uint  Get_NumKeyFrames() { return m_iNumKeyFrames; }

	vector<KEYFRAME> Get_KeyFrames() { return m_KeyFrames; }

	

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const class CModel* pModel);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, CChannel* pNextAniChannel , _double* pCurrentTrackPosition,  _bool isTransitioning, _float fTimeDelta ,_double TransitionTime);

private:
	_char				m_szName[MAX_PATH] = {};				// �� ä���� �̸�

	_uint				m_iBoneIndex = { 0 };					// �� ä���� ������ ���ŵɶ� ���� � ���� ����� �ٲ��ٱ�? � ���� ���� �ε���
	
	_uint				m_iNumKeyFrames = {};					// �� �ִϸ��̼���  Ű  ������ ����

	vector<KEYFRAME>	m_KeyFrames;							// �� Ű �����ӿ� ���� ������ ��Ƶδ� ����

	_float				m_fTransitionTime = { 0.f };			// �ִϸ��̼� ��ȭ �ð�

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const class CModel* pModel);
	virtual void Free() override;
};

END