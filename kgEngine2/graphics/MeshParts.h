/// <summary>
/// メッシュパーツクラス。
/// </summary>

#pragma once

#include "tkFile/TkmFile.h"
#include "StructuredBuffer.h"
#include "RenderMode.h"
#include "graphics/shadow/CascadeShadowMap.h"

class RenderContext;
class Skeleton;
class Material;



/// <summary>
/// メッシュ
/// </summary>
struct SMesh {
	VertexBuffer m_vertexBuffer;						//頂点バッファ。
	std::vector< IndexBuffer* >		m_indexBufferArray;	//インデックスバッファ。
	std::vector< Material* >		m_materials;			//マテリアル。
	std::vector<int>				skinFlags;				//スキンを持っているかどうかのフラグ。
};

/// <summary>
/// メッシュパーツ。
/// </summary>
class MeshParts {
public:
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~MeshParts();
	/// <summary>
	/// tkmファイルから初期化
	/// </summary>
	/// <param name="tkmFile">tkmファイル。</param>
	/// /// <param name="fxFilePath">fxファイルのファイルパス</param>
	/// <param name="vsEntryPointFunc">頂点シェーダーのエントリーポイントの関数名</param>
	/// <param name="psEntryPointFunc">ピクセルシェーダーのエントリーポイントの関数名</param>
	void InitFromTkmFile(
		const TkmFile& tkmFile,
		const wchar_t* fxFilePath,
		const char* vsEntryPointFunc,
		const char* psEntryPointFunc,
		void* expandData,
		int expandDataSize
	) ;
	/// <summary>
	/// 描画。
	/// </summary>
	/// <param name="rc">レンダリングコンテキスト</param>
	/// <param name="mWorld">ワールド行列</param>
	/// <param name="mView">ビュー行列</param>
	/// <param name="mProj">プロジェクション行列</param>
	/// <param name="light">ライト</param>
	void Draw(RenderContext& rc, const Matrix& mWorld, const Matrix& mView, const Matrix& mProj,EnRenderMode renderMode, int shadowMapNumber);
	/// <summary>
	/// スケルトンを関連付ける。
	/// </summary>
	/// <param name="skeleton">スケルトン</param>
	void BindSkeleton(Skeleton& skeleton);
	/// <summary>
	/// シャドウレシーバーのフラグを設定
	/// </summary>
	/// <param name="isFlag"></param>
	void SetShadowReciever(bool isFlag)
	{
		m_isShadowReciever = isFlag;
	}
	/// <summary>
	/// シャドウレシーバーかどうかを取得
	/// </summary>
	/// <returns></returns>
	bool GetShadowReciever()
	{
		return m_isShadowReciever;
	}
private:
	/// <summary>
	/// tkmメッシュからメッシュを作成。
	/// </summary>
	/// <param name="mesh">メッシュ</param>
	/// <param name="meshNo">メッシュ番号</param>
	/// <param name="fxFilePath">fxファイルのファイルパス</param>
	/// <param name="vsEntryPointFunc">頂点シェーダーのエントリーポイントの関数名</param>
	/// <param name="psEntryPointFunc">ピクセルシェーダーのエントリーポイントの関数名</param>
	void CreateMeshFromTkmMesh(
		const TkmFile::SMesh& mesh, 
		int meshNo,
		const wchar_t* fxFilePath,
		const char* vsEntryPointFunc,
		const char* psEntryPointFunc );

	/// <summary>
	/// ディスクリプタヒープを作成。
	/// </summary>
	void CreateDescriptorHeaps();
private:
	/// <summary>
	/// 定数バッファ。
	/// </summary>
	/// <remarks>
	/// この構造体を変更したら、SimpleModel.fxのCBも変更するように。
	/// </remarks>
	struct SConstantBuffer {
		Matrix mWorld;		//ワールド行列。
		Matrix mView;		//ビュー行列。
		Matrix mProj;		//プロジェクション行列。
		Matrix mLightView;		//ライトビュー行列
		Matrix mLightProj;		//ライトプロジェクション行列
		Matrix mLightViewProj[CascadeShadowMap::SHADOWMAP_NUM];	//ライトビュープロジェクション行列
		Vector4 mFarList[CascadeShadowMap::SHADOWMAP_NUM];
		int isShadowReciever;	//シャドウレシーバーのフラグ
		int shadowMapNumber = 0;	//何番目のシャドウマップにレンダリングするか
	};
	ConstantBuffer m_commonConstantBuffer[CascadeShadowMap::SHADOWMAP_NUM + 1];				//メッシュ共通の定数バッファ、通常描画とカスケードシャドウ用に定数バッファを用意する
	ConstantBuffer m_expandConstantBuffer;				//ユーザー拡張用の定数バッファ
	StructuredBuffer m_boneMatricesStructureBuffer;	//ボーン行列の構造化バッファ。
	std::vector< SMesh* > m_meshs;							//メッシュ。
	typedef std::vector<DescriptorHeap> DescriptorHeapList;		//ディスクリプタヒープ。
	DescriptorHeapList m_descriptorHeapList[CascadeShadowMap::SHADOWMAP_NUM + 1];					//通常描画とカスケードシャドウ用にディスクリプタヒープを用意する
	Skeleton* m_skeleton = nullptr;	//スケルトン。
	void* m_expandData = nullptr;	//ユーザー拡張データ。
	bool m_isInitDescriptorHeap = false;	//ディスクリプタヒープを初期化したか？
	bool m_isShadowReciever = true;		//シャドウレシーバーのフラグ
};