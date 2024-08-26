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
	/* dx�� �˾Ƽ� �Ҵ��س� ���� �迭�� ������ ��ǥ�ϴ� �İ�ü.  */
	ID3D11Buffer*			m_pVB = { nullptr };		// ���ؽ� ����
	ID3D11Buffer*			m_pIB = { nullptr };		// �ε��� ����

	D3D11_BUFFER_DESC			m_BufferDesc{};
	D3D11_SUBRESOURCE_DATA		m_InitialData{};



public:
	_uint							m_iNumVertexBuffers = {};     // ���ؽ����۰� ���
	_uint							m_iVertexStride = { 0 };	  // ���ؽ� ũ�Ⱑ ���̴�
	_uint							m_iNumVertices = { 0 };		  // ���ؽ� ������ ���



	_uint							m_iNumFaces = {};			// ��� ���� �׸���


	_uint							m_iIndexStride = { 0 };		// �ε����� ũ��
	_uint							m_iNumIndices = { 0 };		// �ε��� ����

	DXGI_FORMAT						m_eIndexFormat = {};		// �ε��� ������ ����?
	D3D11_PRIMITIVE_TOPOLOGY		m_eTopology = {};			// ����������� �׸��� ex) �ﰢ��.. �� ���ñ���


	_float3*						m_pVerticesPos = { nullptr };


public:
	_uint		m_iNumVerticesX;
	_uint		m_iNumVerticesZ;


protected:
	/* ����, �ε��� ���۸� �Ҵ��Ѵ�. */
	HRESULT Create_Buffer(ID3D11Buffer** ppOut);


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END