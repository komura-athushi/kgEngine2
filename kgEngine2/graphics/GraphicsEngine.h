#pragma once


#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>

#include "DirectXTK/Inc/DDSTextureLoader.h"
#include "DirectXTK/Inc/ResourceUploadBatch.h"

#include "RenderContext.h"

#include "RenderTarget.h"

#include "Sprite.h"

#include "PostEffect\PostEffect.h"

#include "shadow/ShadowMap.h"

#include "shadow/CascadeShadowMap.h"

#include "Camera.h"

/// <summary>
/// DirectX12に依存するグラフィックスエンジン
/// </summary>
class GraphicsEngine {
public:
	enum { FRAME_BUFFER_COUNT = 2 };						//フレームバッファの数。
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~GraphicsEngine();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <remarks>
	/// 本関数を呼び出すことでDirectX12の初期化が行われます。
	/// </remarks>
	/// <param name="hwnd">Windowハンドル</param>
	/// <param name="frameBufferwidth">フレームバッファの幅</param>
	/// <param name="frameBufferHeight">フレームバッファの高さ</param>
	/// <returns>falseが返ってきたら作成に失敗。</returns>
	bool Init(HWND hwnd, UINT frameBufferWidth, UINT frameBufferHeight);
	/// <summary>
	/// レンダリング開始。
	/// </summary>
	/// <remarks>
	/// 1フレームのレンダリングの開始時に呼び出してください。
	/// </remarks>
	void BeginRender();
	/// <summary>
	/// レンダリング終了。
	/// </summary>
	/// <remarks>
	/// 1フレームのレンダリングの終了時に呼び出してください。
	/// </remarks>
	void EndRender();
	/// <summary>
	/// ディファードレンダリングの開始前に呼び出して下さい
	/// </summary>
	void BeginDeferredRender();
	/// <summary>
	/// ディファードレンダリングでモデルのドローが終わったら呼び出してください
	/// </summary>
	void EndModelDraw();
	/// <summary>
	/// ポストエフェクトかける
	/// </summary>
	void RendertoPostEffect();
	/// <summary>
	/// シャドウマップを描画する
	/// </summary>
	void RendertoShadow();
	/// <summary>
	/// レンダリングターゲットをフレームバッファに変更する。
	/// </summary>
	/// <param name="rc"></param>
	void ChangeRenderTargetToFrameBuffer(RenderContext& rc)
	{
		rc.SetRenderTarget(m_currentFrameBufferRTVHandle, m_currentFrameBufferDSVHandle);
	}
	/// <summary>
	/// レンダリングターゲットを設定
	/// </summary>
	/// <param name="numRT"></param>
	/// <param name="renderTargets"></param>
	void SetRenderTarget(int numRT, RenderTarget* renderTargets[])
	{
		m_renderContext.SetRenderTargets(numRT, renderTargets);
	}
	/// <summary>
	/// D3Dデバイスを取得。
	/// </summary>
	/// <returns></returns>
	ID3D12Device* GetD3DDevice()
	{
		return m_d3dDevice;
	}
	/// <summary>
	/// バックバッファの番号を取得。
	/// </summary>
	/// <returns>バックバッファの番号。</returns>
	UINT GetBackBufferIndex() const
	{
		return m_frameIndex;
	}
	/// <summary>
	/// コマンドキューを取得。
	/// </summary>
	/// <returns></returns>
	ID3D12CommandQueue* GetCommandQueue() const
	{
		return m_commandQueue;
	}

	/// <summary>
	/// CBR_SRVのディスクリプタのサイズを取得。
	/// </summary>
	/// <returns></returns>
	UINT GetCbrSrvDescriptorSize() const
	{
		return m_cbrSrvDescriptorSize;
	}
	/// <summary>
	/// レンダリングコンテキストを取得。
	/// </summary>
	/// <returns></returns>
	RenderContext& GetRenderContext()
	{
		return m_renderContext;
	}
	/// <summary>
	/// フレームバッファの幅を取得。
	/// </summary>
	/// <returns>フレームバッファの幅。</returns>
	UINT GetFrameBufferWidth() const
	{
		return m_frameBufferWidth;
	}
	/// <summary>
	/// フレームバッファの高さを取得。
	/// </summary>
	/// <returns>フレームバッファの高さ。</returns>
	UINT GetFrameBufferHeight() const
	{
		return m_frameBufferHeight;
	}
	/// <summary>
	/// メインレンダリングターゲットを取得
	/// </summary>
	/// <returns></returns>
	RenderTarget& GetMainRenderTarget()
	{
		return m_mainRenderTarget;
	}
	/// <summary>
	/// シャドウマップを取得
	/// </summary>
	/// <returns></returns>
	ShadowMap* GetShadowMap()
	{
		return m_shadowMap;
	}
	/// <summary>
	/// カスケードシャドウマップを取得
	/// </summary>
	/// <returns></returns>
	CascadeShadowMap* GetCascadeShadowMap()
	{
		return m_cascadeShadowMap;
	}
	//フレームバッファの深度ステンシルビューを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCpuDescriptorHandle()
	{
		return m_currentFrameBufferDSVHandle;
	}
private:
	/// <summary>
	/// D3Dデバイスの作成。
	/// </summary>
	/// <returns>trueが返ってきたら作成に成功。</returns>
	bool CreateD3DDevice( IDXGIFactory4* dxgiFactory );
	/// <summary>
	/// コマンドキューの作成。
	/// </summary>
	/// <returns>trueが返ってきたら作成に成功。</returns>
	bool CreateCommandQueue();
	/// <summary>
	/// スワップチェインの作成
	/// </summary>
	/// <param name="hwnd">Windowハンドル</param>
	/// <param name="frameBufferWidth">フレームバッファの幅</param>
	/// <param name="frameBufferHeight">フレームバッファの高さ</param>
	/// <param name="dxgiFactory">DirectX グラフィックス インフラストラクチャー</param>
	/// <returns>trueが返ってきたら作成に成功。</returns>
	bool CreateSwapChain(
		HWND hwnd,
		UINT frameBufferWidth,
		UINT frameBufferHeight,
		IDXGIFactory4* dxgiFactory
	);
	/// <summary>
	/// DirectX グラフィックス インフラストラクチャーの作成。
	/// </summary>
	/// <remarks>
	/// DirectX グラフィックス インフラストラクチャーは
	/// カーネル モード ドライバーおよびシステム ハードウェアと通信するためのインターフェースです。 
	/// </remarks>
	/// <returns>作成されたDXGI</returns>
	IDXGIFactory4* CreateDXGIFactory();
	/// <summary>
	/// フレームバッファ用のディスクリプタヒープを作成。
	/// </summary>
	/// <returns>trueが返ってきたら作成に成功。</returns>
	bool CreateDescriptorHeapForFrameBuffer();
	/// <summary>
	/// フレームバッファ用のレンダリングターゲットビューを作成。
	/// </summary>
	/// <returns>trueが返ってきたら作成に成功。</returns>
	bool CreateRTVForFameBuffer();
	/// <summary>
	/// フレームバッファ用の深度ステンシルビューを作成。
	/// </summary>
	/// <param name="frameBufferWidth">フレームバッファの幅</param>
	/// <param name="frameBufferHeight">フレームバッファの高さ</param>
	/// <returns>trueが返ってきたら作成に成功。</returns>
	bool CreateDSVForFrameBuffer( UINT frameBufferWidth,UINT frameBufferHeight );
	/// <summary>
	/// コマンドリストの作成。
	/// </summary>
	/// <returns>trueが返ってきたら作成に成功。</returns>
	bool CreateCommandList();
	/// <summary>
	/// GPUとの同期オブジェクト作成
	/// </summary>
	/// <returns>trueが返ってきたら作成に成功。</returns>
	bool CreateSynchronizationWithGPUObject();
	/// <summary>
	/// 描画の完了待ち。
	/// </summary>
	void WaitDraw();


private:
	//GPUベンダー定義。
	enum GPU_Vender {
		GPU_VenderNvidia,	//NVIDIA
		GPU_VenderAMD,		//Intel
		GPU_VenderIntel,	//AMD
		Num_GPUVender,
	};

	ID3D12Device* m_d3dDevice = nullptr;					//D3Dデバイス。
	ID3D12CommandQueue* m_commandQueue = nullptr;			//コマンドキュー。
	IDXGISwapChain3* m_swapChain = nullptr;					//スワップチェイン。
	ID3D12DescriptorHeap* m_rtvHeap = nullptr;				//レンダリングターゲットビューのディスクリプタヒープ。
	ID3D12DescriptorHeap* m_dsvHeap = nullptr;				//深度ステンシルビューのディスクリプタヒープ。
	ID3D12CommandAllocator* m_commandAllocator = nullptr;	//コマンドアロケータ。
	ID3D12GraphicsCommandList* m_commandList = nullptr;		//コマンドリスト。
	ID3D12PipelineState* m_pipelineState = nullptr;			//パイプラインステート。
	int m_currentBackBufferIndex = 0;						//現在のバックバッファの番号。
	UINT m_rtvDescriptorSize = 0;							//フレームバッファのディスクリプタのサイズ。
	UINT m_dsvDescriptorSize = 0;							//深度ステンシルバッファのディスクリプタのサイズ。
	UINT m_cbrSrvDescriptorSize = 0;						//CBR_SRVのディスクリプタのサイズ。
	ID3D12Resource* m_renderTargets[FRAME_BUFFER_COUNT] = { nullptr };	//フレームバッファ用のレンダリングターゲット。
	ID3D12Resource* m_depthStencilBuffer = nullptr;	//深度ステンシルバッファ。
	D3D12_VIEWPORT m_viewport;			//ビューポート。
	D3D12_RECT m_scissorRect;			//シザリング矩形。
	RenderContext m_renderContext;		//レンダリングコンテキスト。
	// GPUとの同期で使用する変数。
	UINT m_frameIndex = 0;
	HANDLE m_fenceEvent = nullptr;
	ID3D12Fence* m_fence = nullptr;
	UINT64 m_fenceValue = 0;
	UINT m_frameBufferWidth = 0;		//フレームバッファの幅。
	UINT m_frameBufferHeight = 0;		//フレームバッファの高さ。
	Camera m_camera2D;					//2Dカメラ。
	Camera m_camera3D;					//3Dカメラ。
	D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferRTVHandle;		//現在書き込み中のフレームバッファのレンダリングターゲットビューのハンドル。
	D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferDSVHandle;		//現在書き込み中のフレームバッファの深度ステンシルビューのハンドル
	RenderTarget m_albedRT;				//アルベドマップ
	RenderTarget m_normalRT;			//法線マップ
	RenderTarget m_worldPosRT;			//ワールド座標マップ
	RenderTarget m_shadowColorRT;		//影を落とす場所を決めるマップ
	RenderTarget m_depthRT;				//深値マップ
	Sprite m_defferdSprite;				//ディファードレンダリングの為のテクスチャ
	struct SDirectionLight {
		Vector4 direction;		//ライトの方向。
		Vector4 lightcolor;			//ライトのカラー。
	};
	SDirectionLight m_dirLight;			//ライト
	RenderTarget m_mainRenderTarget;	//メインレンダリングターゲット
	PostEffect m_postEffect;			//ポストエフェクト
	Sprite m_copyMainRtToFrameBufferSprite;		//メインレンダリングターゲットをコピーするためのスプライト
	ShadowMap* m_shadowMap = nullptr;
	CascadeShadowMap* m_cascadeShadowMap = nullptr;
};

extern GraphicsEngine* g_graphicsEngine;	//グラフィックスエンジン
extern Camera* g_camera2D;					//2Dカメラ。
extern Camera* g_camera3D;					//3Dカメラ。
