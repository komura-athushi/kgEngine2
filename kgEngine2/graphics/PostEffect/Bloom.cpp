#include "stdafx.h"
#include "Bloom.h"
	
struct SSimpleVertex {
	Vector4 pos;
	Vector2 tex;
};

void Bloom::Init()
{
	//ルートシグネチャを作成。
	m_rootSignature.Init(
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER);

	//シェーダーを初期化。
	InitShaders();
	//レンダリングターゲットを初期化。
	InitRenderTargets();
	//パイプラインステートを初期化。
	InitPipelineState();

	for (auto& cb : m_blurParamCB) {
		//定数バッファを初期化。
		cb.Init(sizeof(m_blurParam), nullptr);
	}
	//ディスクリプタヒープを作成。
	CreateDescriptorHeap();

}
void Bloom::InitRenderTargets()
{
	int w = g_graphicsEngine->GetFrameBufferWidth();
	int h = g_graphicsEngine->GetFrameBufferHeight();
	//輝度抽出用のレンダリングターゲットを作成。
	bool result = m_luminanceRT.Create(w, h, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN);

	//ダウンサンプリング用のレンダリングターゲットを作成する。
	for (int i = 0; i < NUM_DOWN_SAMPLING_RT / 2; i++) {
		int shift = i + 1;
		int baseIndex = i * 2;
		//横ブラー用。
		result = m_downSamplingRT[baseIndex].Create(
			w >> shift,
			h >> (shift - 1),
			1,
			1,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_UNKNOWN);

		//縦ブラー用。
		result = m_downSamplingRT[baseIndex + 1].Create(
			w >> shift,
			h >> shift,
			1,
			1,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_UNKNOWN);
	}
	//ボケ画像合成用のレンダリングターゲットを作成。
	result = m_combineRT.Create(w >> 2, h >> 2, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN);
}
void Bloom::InitShaders()
{
	//シェーダーをロード。
	m_vs.LoadVS(L"Assets/shader/bloom.fx", "VSMain");
	m_psLuminance.LoadPS(L"Assets/shader/bloom.fx", "PSSamplingLuminance");
	m_vsXBlur.LoadVS(L"Assets/shader/bloom.fx", "VSXBlur");
	m_vsYBlur.LoadVS(L"Assets/shader/bloom.fx", "VSYBlur");
	m_psBlur.LoadPS(L"Assets/shader/bloom.fx", "PSBlur");
	m_psCombine.LoadPS(L"Assets/shader/bloom.fx", "PSCombine");
	m_copyVS.LoadVS(L"Assets/shader/copy.fx", "VSMain");
	m_copyPS.LoadPS(L"Assets/shader/copy.fx", "PSMain");

}
void Bloom::InitPipelineState()
{
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	//パイプラインステートを作成。
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vs.GetCompiledBlob());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_psLuminance.GetCompiledBlob());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
	psoDesc.SampleDesc.Count = 1;
	m_samplingLuminancePipelineState.Init(psoDesc);

	//Xブラー用のパイプラインステート。
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsXBlur.GetCompiledBlob());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_psBlur.GetCompiledBlob());
	m_xblurLuminancePipelineState.Init(psoDesc);

	//yブラー用のパイプラインステート。
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vsYBlur.GetCompiledBlob());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_psBlur.GetCompiledBlob());
	m_yblurLuminancePipelineState.Init(psoDesc);

	//ボケ画像合成用のパイプラインステート。
	psoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vs.GetCompiledBlob());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_psCombine.GetCompiledBlob());
	m_combineBokeImagePipelineState.Init(psoDesc);

	//最終合成用のパイプラインステート。
	psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_copyVS.GetCompiledBlob());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_copyPS.GetCompiledBlob());
	m_combineMainRenderTargetPipelineState.Init(psoDesc);
}

void Bloom::UpdateWeight(float dispersion)
{
	float total = 0;
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		m_blurParam.weights[i] = expf(-0.5f * (float)(i * i) / dispersion);
		total += 2.0f * m_blurParam.weights[i];

	}
	// 規格化
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		m_blurParam.weights[i] /= total;
	}
}
void Bloom::SamplingLuminance(RenderContext& rc12)
{
	//メインレンダリングターゲットをテクスチャとして利用するので
	//利用できるようになるまで待機する。
//	rc12.WaitUntilFinishDrawingToRenderTarget(ge12.GetMainRenderTarget());
	rc12.WaitUntilToPossibleSetRenderTarget(m_luminanceRT);
	//パイプラインステートを設定。
	rc12.SetPipelineState(m_samplingLuminancePipelineState);
	//レンダリングターゲットを輝度抽出用に切り替える。
	rc12.SetRenderTargetAndViewport(m_luminanceRT);
	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	rc12.ClearRenderTargetView(m_luminanceRT, clearColor);
	//シェーダーリソースビューと定数バッファをセットする。
	rc12.SetDescriptorHeap(m_sampleLuminanceDiscripterHeap);
	//ドロドロ。
	rc12.DrawIndexed(4);
	rc12.WaitUntilFinishDrawingToRenderTarget(m_luminanceRT);
}
void Bloom::CreateDescriptorHeap()
{
	m_sampleLuminanceDiscripterHeap.RegistShaderResource(0, g_graphicsEngine->GetMainRenderTarget().GetRenderTargetTexture());
	m_sampleLuminanceDiscripterHeap.Commit();

	m_combineBokeImageDescriptorHeap.RegistShaderResource(0, m_downSamplingRT[3].GetRenderTargetTexture());
	m_combineBokeImageDescriptorHeap.RegistShaderResource(1, m_downSamplingRT[5].GetRenderTargetTexture());
	m_combineBokeImageDescriptorHeap.RegistShaderResource(2, m_downSamplingRT[7].GetRenderTargetTexture());
	m_combineBokeImageDescriptorHeap.RegistShaderResource(3, m_downSamplingRT[9].GetRenderTargetTexture());
	m_combineBokeImageDescriptorHeap.Commit();


	RenderTarget* prevRt = &m_luminanceRT;

	int rtNo = 0;
	for (int i = 0; i < NUM_DOWN_SAMPLING_RT / 2; i++) {
		//Xブラー。
		{
			m_downSampleDescriptorHeap[rtNo].RegistShaderResource(0, prevRt->GetRenderTargetTexture());
			m_downSampleDescriptorHeap[rtNo].RegistConstantBuffer(0, m_blurParamCB[rtNo]);
			m_downSampleDescriptorHeap[rtNo].Commit();
		}
		prevRt = &m_downSamplingRT[rtNo];
		rtNo++;
		//Yブラー。
		{
			m_downSampleDescriptorHeap[rtNo].RegistConstantBuffer(0, m_blurParamCB[rtNo]);
			m_downSampleDescriptorHeap[rtNo].RegistShaderResource(0, prevRt->GetRenderTargetTexture());
			m_downSampleDescriptorHeap[rtNo].Commit();
		}
		prevRt = &m_downSamplingRT[rtNo];
		rtNo++;
	}

	m_combineMainRenderTargetDescriptorHeap.RegistShaderResource(0, m_combineRT.GetRenderTargetTexture());
	m_combineMainRenderTargetDescriptorHeap.Commit();
}
void Bloom::BlurLuminanceTexture(RenderContext& rc12)
{
	RenderTarget* prevRt = &m_luminanceRT;

	int rtIndex = 0;
	for (int i = 0; i < NUM_DOWN_SAMPLING_RT / 2; i++) {
		//Xブラー。
		{
			m_blurParam.offset.x = 16.0f / prevRt->GetWidth();
			m_blurParam.offset.y = 0.0f;
			m_blurParamCB[rtIndex].Update(&m_blurParam);

			rc12.WaitUntilToPossibleSetRenderTarget(m_downSamplingRT[rtIndex]);
			rc12.SetPipelineState(m_xblurLuminancePipelineState);
			rc12.SetRenderTargetAndViewport(m_downSamplingRT[rtIndex]);

			//シェーダーリソースビューと定数バッファをセットする。
			auto& dheap = m_downSampleDescriptorHeap[rtIndex];
			rc12.SetDescriptorHeap(m_downSampleDescriptorHeap[rtIndex]);

			rc12.DrawIndexed(4);
			rc12.WaitUntilFinishDrawingToRenderTarget(m_downSamplingRT[rtIndex]);
		}
		prevRt = &m_downSamplingRT[rtIndex];
		rtIndex++;
		//Yブラー。
		{
			m_blurParam.offset.x = 0.0f;
			m_blurParam.offset.y = 16.0f / prevRt->GetWidth();
			m_blurParamCB[rtIndex].Update(&m_blurParam);

			rc12.WaitUntilToPossibleSetRenderTarget(m_downSamplingRT[rtIndex]);
			rc12.SetRenderTargetAndViewport(m_downSamplingRT[rtIndex]);
			rc12.SetPipelineState(m_yblurLuminancePipelineState);

			//シェーダーリソースビューと定数バッファをセットする。
			rc12.SetDescriptorHeap(m_downSampleDescriptorHeap[rtIndex]);

			rc12.DrawIndexed(4);
			rc12.WaitUntilFinishDrawingToRenderTarget(m_downSamplingRT[rtIndex]);
		}
		prevRt = &m_downSamplingRT[rtIndex];
		rtIndex++;
	}
}
void Bloom::CombineBokeImage(RenderContext& rc12)
{
	rc12.WaitUntilToPossibleSetRenderTarget(m_combineRT);
	rc12.SetPipelineState(m_combineBokeImagePipelineState);
	rc12.SetRenderTargetAndViewport(m_combineRT);

	//シェーダーリソースビューと定数バッファをセットする。
	rc12.SetDescriptorHeap(m_combineBokeImageDescriptorHeap);
	rc12.DrawIndexed(4);

	rc12.WaitUntilFinishDrawingToRenderTarget(m_combineRT);

}
void Bloom::CombineMainRenderTarget(RenderContext& rc12)
{
	rc12.WaitUntilToPossibleSetRenderTarget(g_graphicsEngine->GetMainRenderTarget());
	rc12.SetPipelineState(m_combineMainRenderTargetPipelineState);
	rc12.SetRenderTargetAndViewport(g_graphicsEngine->GetMainRenderTarget());

	//シェーダーリソースビューと定数バッファをセットする。
	rc12.SetDescriptorHeap(m_combineMainRenderTargetDescriptorHeap);
	rc12.DrawIndexed(4);
	//	rc12.WaitUntilFinishDrawingToRenderTarget(ge12.GetMainRenderTarget());
}
void Bloom::Render(RenderContext& rc)
{
	//重みを更新。
	UpdateWeight(25.0f);

	//ルートシグネチャを設定。
	rc.SetRootSignature(m_rootSignature);

	//輝度を抽出する
	SamplingLuminance(rc);
	//輝度テクスチャをぼかす。
	BlurLuminanceTexture(rc);
	//ボケ画像を合成する。
	CombineBokeImage(rc);
	//最終合成
	CombineMainRenderTarget(rc);
}