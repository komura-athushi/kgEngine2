#pragma once

#include "Indexbuffer.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"

class Texture;

/// <summary>
/// スプライトクラス。
/// </summary>
class Sprite  {
public:
	static const Vector2	DEFAULT_PIVOT;					//!<ピボット。
	virtual ~Sprite();
	/// <summary>
	/// 初期化。
	/// </summary>
	/// <param name="ddsFilePath">ddsファイルのファイルパス</param>
	/// <param name="w">幅</param>
	/// <param name="h">高さ</param>
	void Init(const char* ddsFilePath, float w, float h);
	/// <summary>
	/// 更新。
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="rot">回転</param>
	/// <param name="scale">拡大率</param>
	/// <param name="pivot">
	/// ピボット
	/// 0.5, 0.5で画像の中心が基点。
	/// 0.0, 0.0で画像の左下。
	/// 1.0, 1.0で画像の右上。
	/// UnityのuGUIに準拠。
	/// </param>
	void Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale, const Vector2& pivot = DEFAULT_PIVOT);
	/// <summary>
	/// 描画。
	/// </summary>
	/// <param name="renderContext">レンダリングコンテキスト/param>
	void Draw(RenderContext& renderContext);
private:
	IndexBuffer m_indexBuffer;		//インデックスバッファ。
	VertexBuffer m_vertexBuffer;	//頂点バッファ。
	Texture m_texture;				//テクスチャ。
	Vector3 m_position ;			//座標。
	Vector2 m_size;					//サイズ。
	Quaternion m_rotation ;			//回転。
	Matrix m_world;					//ワールド行列。

	struct LocalConstantBuffer {
		Matrix mvp;
		Vector4 mulColor;
	};
	LocalConstantBuffer m_constantBufferCPU;	//CPU側の定数バッファ。
	ConstantBuffer		m_constantBufferGPU;	//GPU側の定数バッファ。
	DescriptorHeap		m_descriptorHeap;		//ディスクリプタヒープ。
	RootSignature		m_rootSignature;		//ルートシグネチャ。
	PipelineState		m_pipelineState;		//パイプラインステート。
	Shader				m_vs;					//頂点シェーダー。
	Shader				m_ps;					//ピクセルシェーダー。
};