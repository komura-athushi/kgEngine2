#include "stdafx.h"
#include "Sprite.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

	namespace {
		struct SSimpleVertex {
			Vector4 pos;
			Vector2 tex;
		};
	}
	const Vector2	Sprite::DEFAULT_PIVOT = { 0.5f, 0.5f };
	Sprite::~Sprite()
	{
	}
	void Sprite::Init(const char* ddsFilePath, float w, float h)
	{
		wchar_t wddsFilePath[1024];
		mbstowcs(wddsFilePath, ddsFilePath, 1023);
		m_texture.InitFromDDSFile(wddsFilePath);

		m_size.x = w;
		m_size.y = h;

		float halfW = w * 0.5f;
		float halfH = h * 0.5f;
		//���_�o�b�t�@�̃\�[�X�f�[�^�B
		SSimpleVertex vertices[] =
		{
			{
				Vector4(-halfW, -halfH, 0.0f, 1.0f),
				Vector2(0.0f, 1.0f),
			},
			{
				Vector4(halfW, -halfH, 0.0f, 1.0f),
				Vector2(1.0f, 1.0f),
			},
			{
				Vector4(-halfW, halfH, 0.0f, 1.0f),
				Vector2(0.0f, 0.0f)
			},
			{
				Vector4(halfW, halfH, 0.0f, 1.0f),
				Vector2(1.0f, 0.0f)
			}

		};
		short indices[] = { 0,1,2,3 };
	
		m_vertexBuffer.Init(sizeof(vertices), sizeof(vertices[0]));
		m_vertexBuffer.Copy(vertices);

		m_indexBuffer.Init(sizeof(indices), sizeof(indices[0]));
		m_indexBuffer.Copy(indices);


		//�萔�o�b�t�@�̏������B
		m_constantBufferGPU.Init(sizeof(m_constantBufferCPU), nullptr);
		//���[�g�V�O�l�`���̏������B
		m_rootSignature.Init(
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP);

		//�V�F�[�_�[�����[�h����B
		m_vs.LoadVS(L"Assets/shader/sprite.fx", "VSMain");
		m_ps.LoadPS(L"Assets/shader/sprite.fx", "PSMain");

		//�p�C�v���C���X�e�[�g�̏������B
		{
			// ���_���C�A�E�g���`����B
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			//�p�C�v���C���X�e�[�g���쐬�B
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
			psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			psoDesc.pRootSignature = m_rootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vs.GetCompiledBlob());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_ps.GetCompiledBlob());
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
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleDesc.Count = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_pipelineState.Init(psoDesc);
		}
	}
	void Sprite::Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale, const Vector2& pivot)
	{
		//�s�{�b�g���l���ɓ��ꂽ���s�ړ��s����쐬�B
		//�s�{�b�g�͐^�񒆂�0.0, 0.0�A���オ-1.0f, -1.0�A�E����1.0�A1.0�ɂȂ�悤�ɂ���B
		Vector2 localPivot = pivot;
		localPivot.x -= 0.5f;
		localPivot.y -= 0.5f;
		localPivot.x *= -2.0f;
		localPivot.y *= -2.0f;
		//�摜�̃n�[�t�T�C�Y�����߂�B
		Vector2 halfSize = m_size;
		halfSize.x *= 0.5f;
		halfSize.y *= 0.5f;
		Matrix mPivotTrans;

		mPivotTrans.MakeTranslation(
			{ halfSize.x * localPivot.x, halfSize.y * localPivot.y, 0.0f }
		);
		Matrix mTrans, mRot, mScale;
		mTrans.MakeTranslation(pos);
		mRot.MakeRotationFromQuaternion(rot);
		mScale.MakeScaling(scale);
		m_world = mPivotTrans * mScale;
		m_world = m_world * mRot;
		m_world = m_world * mTrans;
	}
	void Sprite::Draw(RenderContext& renderContext)
	{
		Matrix viewMatrix = g_camera2D->GetViewMatrix();
		Matrix projMatrix = g_camera2D->GetProjectionMatrix();

		m_constantBufferCPU.mvp = m_world * viewMatrix * projMatrix;
		m_constantBufferCPU.mulColor.x = 1.0f;
		m_constantBufferCPU.mulColor.y = 1.0f;
		m_constantBufferCPU.mulColor.z = 1.0f;
		m_constantBufferCPU.mulColor.w = 1.0f;
		//�萔�o�b�t�@���X�V�B
		m_constantBufferGPU.CopyToVRAM(&m_constantBufferCPU);

		//���[�g�V�O�l�`����ݒ�B
		renderContext.SetRootSignature(m_rootSignature);
		//�p�C�v���C���X�e�[�g��ݒ�B
		renderContext.SetPipelineState(m_pipelineState);
		//���_�o�b�t�@��ݒ�B
		renderContext.SetVertexBuffer(m_vertexBuffer);
		//�C���f�b�N�X�o�b�t�@��ݒ�B
		renderContext.SetIndexBuffer(m_indexBuffer);
		//�v���~�e�B�u�g�|���W�[��ݒ肷��B
		renderContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//�萔�o�b�t��ݒ�B
		renderContext.SetConstantBuffer(0, m_constantBufferGPU);
		//�e�N�X�`����ݒ�B
		renderContext.SetShaderResource(0, m_texture);	
		//�`��
		renderContext.DrawIndexed(m_indexBuffer.GetCount());
	}

