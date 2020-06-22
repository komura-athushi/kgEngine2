#pragma once

class Bloom 
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
	/// <summary>
	/// 重みを更新
	/// </summary>
	/// <param name="dispersion"></param>
	void UpdateWeight(float dispersion);
	/// <summary>
	/// レンダリングターゲットを初期化
	/// </summary>
	void InitRenderTargets();
	/// <summary>
	/// シェーダーを初期化
	/// </summary>
	void InitShaders();
	/// <summary>
	/// パイプラインステートを初期化
	/// </summary>
	void InitPipelineState();
	/// <summary>
	/// 輝度を抽出
	/// </summary>
	/// <param name="rc"></param>
	void SamplingLuminance(RenderContext& rc);
	/// <summary>
	/// 輝度テクスチャにブラーをかける
	/// </summary>
	/// <param name="rc"></param>
	void BlurLuminanceTexture(RenderContext& rc);
	/// <summary>
	/// ボケ画像を合成する
	/// </summary>
	/// <param name="rc"></param>
	void CombineBokeImage(RenderContext& rc);
	/// <summary>
	/// レンダリングターゲットへの合成
	/// </summary>
	/// <param name="rc"></param>
	void CombineMainRenderTarget(RenderContext& rc);
	/// <summary>
	/// ディスクリプタヒープの合成
	/// </summary>
	void CreateDescriptorHeap();
private:
	static const int NUM_WEIGHTS = 8;
	static const int NUM_DOWN_SAMPLING_RT = 10;
	struct SBlurParam {
		Vector4 offset;
		float weights[NUM_WEIGHTS];
	};

	RootSignature m_rootSignature;		//ルートシグネチャ

	PipelineState m_samplingLuminancePipelineState;	//輝度抽出ステップのパイプラインステート。
	PipelineState m_xblurLuminancePipelineState;		//x方向に輝度をぼかすステップのパイプラインステート。
	PipelineState m_yblurLuminancePipelineState;		//y方向に輝度をぼかすステップのパイプラインステート。
	PipelineState m_combineBokeImagePipelineState;	//ボケ画像を合成するステップのパイプラインステート。
	PipelineState m_combineMainRenderTargetPipelineState;			//メインレンダリングターゲットへの合成ステップのパイプラインステート。

	RenderTarget m_luminanceRT;	//輝度を抽出するためのレンダリングターゲット。
	RenderTarget m_combineRT;		//ぼかし合成用のレンダリングターゲット。
	RenderTarget m_downSamplingRT[NUM_DOWN_SAMPLING_RT];	//ダウンサンプリング用のレンダリングターゲット。
	SBlurParam m_blurParam;			//ブラー用のパラメータ。
	ConstantBuffer m_cbBlur;
	Shader m_vs;				//頂点シェーダー。
	Shader m_psLuminance;		//輝度抽出パスのピクセルシェーダー。
	Shader m_vsXBlur;			//X方向ブラーパスの頂点シェーダー。
	Shader m_vsYBlur;			//Y方向ブラーパスの頂点シェーダー。
	Shader m_psBlur;			//X方向ブラー/Y方向ブラーパスのピクセルシェーダー。
	Shader m_psCombine;		//合成パスのピクセルシェーダー。
	Shader m_copyVS;			//コピー用の頂点シェーダー。	
	Shader m_copyPS;			//コピー用のピクセルシェーダー。
	ConstantBuffer m_blurParamCB[NUM_DOWN_SAMPLING_RT];	//ブラー用の定数バッファ。
	DescriptorHeap m_sampleLuminanceDiscripterHeap;		//輝度抽出時に使用するディスクリプタヒープ。
	DescriptorHeap m_combineBokeImageDescriptorHeap;		//ボケ画像合成に使用するディスクリプタヒープ。
	DescriptorHeap m_downSampleDescriptorHeap[NUM_DOWN_SAMPLING_RT];	//ダウンサンプリング用のディスクリプタヒープ。
	DescriptorHeap m_combineMainRenderTargetDescriptorHeap;			//メインレンダリングターゲットへの合成に使用するディスクリプタヒープ。
};

