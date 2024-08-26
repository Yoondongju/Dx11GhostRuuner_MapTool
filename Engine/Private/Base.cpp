#include "..\Public\Base.h"

CBase::CBase()
{
}

unsigned int CBase::AddRef()
{
    return ++m_iRefCnt;
}

unsigned int CBase::Release()
{
	if (0 == m_iRefCnt)
	{
		Free();		// 오버라이딩 된 객체의 Free를 호출할것

		delete this;

		return 0;
	}
	else
		return m_iRefCnt--;
}

void CBase::Free()
{
}
