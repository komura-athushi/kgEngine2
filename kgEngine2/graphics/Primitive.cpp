#include "stdafx.h"
#include "Primitive.h"

void Primitive::Init(
	void* srcVertexData,
	int vertexBufferSize,
	int vertexBufferStride,
	void* srcIndexData,
	int indexBufferSize,
	int indexBufferStride,
	D3D12_PRIMITIVE_TOPOLOGY enPrimTopology)
{
	m_vertexBuffer.Init(vertexBufferSize, vertexBufferStride);
	m_indexBuffer.Init(indexBufferSize, indexBufferStride);
	m_vertexBuffer.Copy(srcVertexData);
	m_indexBuffer.Copy(srcIndexData);
	m_topology = enPrimTopology;
}