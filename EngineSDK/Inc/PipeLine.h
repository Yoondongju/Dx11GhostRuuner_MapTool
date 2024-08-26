#pragma once

#include "Base.h"

class CPipeLine final : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;


public:
	void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
	}

public:
	_float4x4 Get_Transform_Float4x4(D3DTRANSFORMSTATE eState) const {
		return m_TransformMatrices[eState];
	}
	_matrix Get_Transform_Matrix(D3DTRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformMatrices[eState]);
	}
	_float4x4 Get_Transform_Inverse_Float4x4(D3DTRANSFORMSTATE eState) const {
		return m_TransformInverseMatrices[eState];
	}
	_matrix Get_Transform_Inverse_Matrix(D3DTRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformInverseMatrices[eState]);
	}

	_float4 Get_CamPosition_Float4() const {
		return m_vCamPosition;
	}
	_vector Get_CamPosition_Vector() const {
		return XMLoadFloat4(&m_vCamPosition);
	}


public:
	HRESULT	Initialize();
	void Update();


private:
	_float4x4	m_TransformMatrices[D3DTRANSFORMSTATE::D3DTS_END];
	_float4x4	m_TransformInverseMatrices[D3DTRANSFORMSTATE::D3DTS_END];
	_float4		m_vCamPosition = {};





public:
	static CPipeLine* Create();
	virtual void Free() override;
};

