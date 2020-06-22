#include "stdafx.h"
#include "PostEffect.h"

struct  SSimpleVertex
{
	Vector4 pos;
	Vector2 tex;
};

void PostEffect::Init()
{
	//ÉuÉãÅ[ÉÄÇèâä˙âª
	m_bloom.Init();

	SSimpleVertex vertices[] =
	{
		{
			Vector4(-1.0f, -1.0f, 0.0f, 1.0f),
			Vector2(0.0f, 1.0f),
		},
		{
			Vector4(1.0f, -1.0f, 0.0f, 1.0f),
			Vector2(1.0f, 1.0f),
		},
		{
			Vector4(-1.0f, 1.0f, 0.0f, 1.0f),
			Vector2(0.0f, 0.0f)
		},
		{
			Vector4(1.0f, 1.0f, 0.0f, 1.0f),
			Vector2(1.0f, 0.0f)
		}

	};

	short indices[] = { 0,1,2,3 };
	m_quadPrimitive.Init(
		vertices,
		sizeof(vertices),
		sizeof(SSimpleVertex),
		indices,
		sizeof(indices),
		2,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void PostEffect::Render(RenderContext& rc)
{
	rc.SetVertexBuffer(m_quadPrimitive.GetVertexBuffer());
	rc.SetIndexBuffer(m_quadPrimitive.GetIndexBuffer());
	rc.SetPrimitiveTopology(m_quadPrimitive.GetPrimitiveTopology());

	m_bloom.Render(rc);
}