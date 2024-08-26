#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers();
	virtual HRESULT Render();


public:
	virtual _bool isPicking(const _fmatrix& WorldMatrix, _float3* pOut) { return false; }




protected:
	/* dx가 알아서 할당해낸 정점 배열의 공간을 대표하는 컴객체.  */
	ID3D11Buffer*			m_pVB = { nullptr };		// 버텍스 버퍼
	ID3D11Buffer*			m_pIB = { nullptr };		// 인덱스 버퍼

	D3D11_BUFFER_DESC			m_BufferDesc{};
	D3D11_SUBRESOURCE_DATA		m_InitialData{};



public:
	_uint							m_iNumVertexBuffers = {};     // 버텍스버퍼가 몇개니
	_uint							m_iVertexStride = { 0 };	  // 버텍스 크기가 몇이니
	_uint							m_iNumVertices = { 0 };		  // 버텍스 갯수가 몇개니



	_uint							m_iNumFaces = {};			// 몇개의 면을 그릴래


	_uint							m_iIndexStride = { 0 };		// 인덱스의 크기
	_uint							m_iNumIndices = { 0 };		// 인덱스 갯수

	DXGI_FORMAT						m_eIndexFormat = {};		// 인덱스 포맷이 뭐지?
	D3D11_PRIMITIVE_TOPOLOGY		m_eTopology = {};			// 무슨모양으로 그릴래 ex) 삼각형.. 점 뭐시기등등


	_float3*						m_pVerticesPos = { nullptr };


public:
	_uint		m_iNumVerticesX;
	_uint		m_iNumVerticesZ;


protected:
	/* 정점, 인덱스 버퍼를 할당한다. */
	HRESULT Create_Buffer(ID3D11Buffer** ppOut);


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END