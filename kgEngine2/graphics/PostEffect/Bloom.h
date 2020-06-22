#pragma once

class Bloom 
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
	/// <summary>
	/// �d�݂��X�V
	/// </summary>
	/// <param name="dispersion"></param>
	void UpdateWeight(float dispersion);
	/// <summary>
	/// �����_�����O�^�[�Q�b�g��������
	/// </summary>
	void InitRenderTargets();
	/// <summary>
	/// �V�F�[�_�[��������
	/// </summary>
	void InitShaders();
	/// <summary>
	/// �p�C�v���C���X�e�[�g��������
	/// </summary>
	void InitPipelineState();
	/// <summary>
	/// �P�x�𒊏o
	/// </summary>
	/// <param name="rc"></param>
	void SamplingLuminance(RenderContext& rc);
	/// <summary>
	/// �P�x�e�N�X�`���Ƀu���[��������
	/// </summary>
	/// <param name="rc"></param>
	void BlurLuminanceTexture(RenderContext& rc);
	/// <summary>
	/// �{�P�摜����������
	/// </summary>
	/// <param name="rc"></param>
	void CombineBokeImage(RenderContext& rc);
	/// <summary>
	/// �����_�����O�^�[�Q�b�g�ւ̍���
	/// </summary>
	/// <param name="rc"></param>
	void CombineMainRenderTarget(RenderContext& rc);
	/// <summary>
	/// �f�B�X�N���v�^�q�[�v�̍���
	/// </summary>
	void CreateDescriptorHeap();
private:
	static const int NUM_WEIGHTS = 8;
	static const int NUM_DOWN_SAMPLING_RT = 10;
	struct SBlurParam {
		Vector4 offset;
		float weights[NUM_WEIGHTS];
	};

	RootSignature m_rootSignature;		//���[�g�V�O�l�`��

	PipelineState m_samplingLuminancePipelineState;	//�P�x���o�X�e�b�v�̃p�C�v���C���X�e�[�g�B
	PipelineState m_xblurLuminancePipelineState;		//x�����ɋP�x���ڂ����X�e�b�v�̃p�C�v���C���X�e�[�g�B
	PipelineState m_yblurLuminancePipelineState;		//y�����ɋP�x���ڂ����X�e�b�v�̃p�C�v���C���X�e�[�g�B
	PipelineState m_combineBokeImagePipelineState;	//�{�P�摜����������X�e�b�v�̃p�C�v���C���X�e�[�g�B
	PipelineState m_combineMainRenderTargetPipelineState;			//���C�������_�����O�^�[�Q�b�g�ւ̍����X�e�b�v�̃p�C�v���C���X�e�[�g�B

	RenderTarget m_luminanceRT;	//�P�x�𒊏o���邽�߂̃����_�����O�^�[�Q�b�g�B
	RenderTarget m_combineRT;		//�ڂ��������p�̃����_�����O�^�[�Q�b�g�B
	RenderTarget m_downSamplingRT[NUM_DOWN_SAMPLING_RT];	//�_�E���T���v�����O�p�̃����_�����O�^�[�Q�b�g�B
	SBlurParam m_blurParam;			//�u���[�p�̃p�����[�^�B
	ConstantBuffer m_cbBlur;
	Shader m_vs;				//���_�V�F�[�_�[�B
	Shader m_psLuminance;		//�P�x���o�p�X�̃s�N�Z���V�F�[�_�[�B
	Shader m_vsXBlur;			//X�����u���[�p�X�̒��_�V�F�[�_�[�B
	Shader m_vsYBlur;			//Y�����u���[�p�X�̒��_�V�F�[�_�[�B
	Shader m_psBlur;			//X�����u���[/Y�����u���[�p�X�̃s�N�Z���V�F�[�_�[�B
	Shader m_psCombine;		//�����p�X�̃s�N�Z���V�F�[�_�[�B
	Shader m_copyVS;			//�R�s�[�p�̒��_�V�F�[�_�[�B	
	Shader m_copyPS;			//�R�s�[�p�̃s�N�Z���V�F�[�_�[�B
	ConstantBuffer m_blurParamCB[NUM_DOWN_SAMPLING_RT];	//�u���[�p�̒萔�o�b�t�@�B
	DescriptorHeap m_sampleLuminanceDiscripterHeap;		//�P�x���o���Ɏg�p����f�B�X�N���v�^�q�[�v�B
	DescriptorHeap m_combineBokeImageDescriptorHeap;		//�{�P�摜�����Ɏg�p����f�B�X�N���v�^�q�[�v�B
	DescriptorHeap m_downSampleDescriptorHeap[NUM_DOWN_SAMPLING_RT];	//�_�E���T���v�����O�p�̃f�B�X�N���v�^�q�[�v�B
	DescriptorHeap m_combineMainRenderTargetDescriptorHeap;			//���C�������_�����O�^�[�Q�b�g�ւ̍����Ɏg�p����f�B�X�N���v�^�q�[�v�B
};

