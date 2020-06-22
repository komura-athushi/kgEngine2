#pragma once
#include "graphics/Primitive.h"
#include "Bloom.h"

class PostEffect
{
public:
	/// <summary>
	/// ������
	/// </summary>
	void Init();
	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="rc"></param>
	void Render(RenderContext& rc);
private:
	Bloom m_bloom;		//�u���[��
	Primitive m_quadPrimitive;		//�l�p�`�v���~�e�B�u
};