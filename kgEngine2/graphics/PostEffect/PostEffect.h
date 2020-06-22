#pragma once
#include "graphics/Primitive.h"
#include "Bloom.h"

class PostEffect
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="rc"></param>
	void Render(RenderContext& rc);
private:
	Bloom m_bloom;		//ブルーム
	Primitive m_quadPrimitive;		//四角形プリミティブ
};