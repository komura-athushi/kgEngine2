#pragma once

#include "tkFile/TkmFile.h"
#include "MeshParts.h"
#include "Skeleton.h"


template <class TExpandData> struct ModelExpandDataInfo {
	TExpandData* m_expandData = nullptr;
	int m_expandDataSize = 0;
};

struct ModelInitData {
	const char* m_tkmFilePath = nullptr;		//tkmファイルパス。
	const char* m_vsEntryPointFunc = "VSMain";	//頂点シェーダーのエントリーポイント。
	const char* m_psEntryPointFunc = "PSMain";	//ピクセルシェーダーのエントリーポイント。
	const char* m_fxFilePath = nullptr;			//.fxファイルのファイルパス。
	void* m_expandConstantBuffer = nullptr;		//ユーザー拡張の定数バッファ。
	int m_expandConstantBufferSize = 0;			//ユーザー拡張の定数バッファのサイズ。
};
/// <summary>
/// モデルクラス。
/// </summary>
class Model {

public:

	/// <summary>
	/// tkmファイルから初期化。
	/// </summary>
	/// <param name="filePath">tkmファイルのファイルパス</param>
	/// <param name="fxFilePath">fxファイルのファイルパス</param>
	/// <param name="vsEntryPointFunc">頂点シェーダーのエントリーポイントの関数名</param>
	/// <param name="psEntryPointFunc">ピクセルシェーダーのエントリーポイントの関数名</param>
	void Init( const ModelInitData& initData );
	/// <summary>
	/// ワールド行列の更新。
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="rot">回転</param>
	/// <param name="scale">拡大率</param>
	void UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale);

	/// <summary>
	/// スケルトンを関連付ける。
	/// </summary>
	/// <param name="skeleton">スケルトン</param>
	void BindSkeleton(Skeleton& skeleton)
	{
		m_meshParts.BindSkeleton(skeleton);
	}
	/// <summary>
	/// スケルトンを関連付ける、Modelの保有してるスケルトンを関連付ける
	/// </summary>
	void BindSkeleton()
	{
		m_meshParts.BindSkeleton(m_skeleton);
	}
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="renderContext">レンダリングコンテキスト</param>
	void Draw(RenderContext& renderContext);
	/// <summary>
	/// ワールド行列を取得。
	/// </summary>
	/// <returns></returns>
	const Matrix& GetWorldMatrix() const
	{
		return m_world;
	}
	//骨取得
	Skeleton& GetSkeleton()
	{
		return m_skeleton;
	}
private:
	void InitSkeleton(const char* filePath);

private:
	Matrix m_world;			//ワールド行列。
	TkmFile m_tkmFile;		//tkmファイル。
	Skeleton m_skeleton;	//スケルトン。
	MeshParts m_meshParts;	//メッシュパーツ。
};
