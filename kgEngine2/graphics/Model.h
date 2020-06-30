#pragma once

#include "tkFile/TkmFile.h"
#include "MeshParts.h"
#include "Skeleton.h"
#include "RenderMode.h"

template <class TExpandData> struct ModelExpandDataInfo {
	TExpandData* m_expandData = nullptr;
	int m_expandDataSize = 0;
};

struct ModelInitData {
	const char* m_tkmFilePath = nullptr;		//tkm�t�@�C���p�X�B
	const char* m_vsEntryPointFunc = "VSMain";	//���_�V�F�[�_�[�̃G���g���[�|�C���g�B
	const char* m_psEntryPointFunc = "PSMain";	//�s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g�B
	const char* m_fxFilePath = nullptr;			//.fx�t�@�C���̃t�@�C���p�X�B
	void* m_expandConstantBuffer = nullptr;		//���[�U�[�g���̒萔�o�b�t�@�B
	int m_expandConstantBufferSize = 0;			//���[�U�[�g���̒萔�o�b�t�@�̃T�C�Y�B
};



/// <summary>
/// ���f���N���X�B
/// </summary>
class Model {

public:

	/// <summary>
	/// tkm�t�@�C�����珉�����B
	/// </summary>
	/// <param name="filePath">tkm�t�@�C���̃t�@�C���p�X</param>
	/// <param name="fxFilePath">fx�t�@�C���̃t�@�C���p�X</param>
	/// <param name="vsEntryPointFunc">���_�V�F�[�_�[�̃G���g���[�|�C���g�̊֐���</param>
	/// <param name="psEntryPointFunc">�s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g�̊֐���</param>
	void Init( const ModelInitData& initData );
	/// <summary>
	/// ���[���h�s��̍X�V�B
	/// </summary>
	/// <param name="pos">���W</param>
	/// <param name="rot">��]</param>
	/// <param name="scale">�g�嗦</param>
	void UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale);

	/// <summary>
	/// �X�P���g�����֘A�t����B
	/// </summary>
	/// <param name="skeleton">�X�P���g��</param>
	void BindSkeleton(Skeleton& skeleton)
	{
		m_meshParts.BindSkeleton(skeleton);
	}
	/// <summary>
	/// �X�P���g�����֘A�t����AModel�ۗ̕L���Ă�X�P���g�����֘A�t����
	/// </summary>
	void BindSkeleton()
	{
		m_meshParts.BindSkeleton(m_skeleton);
	}
	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="renderContext">�����_�����O�R���e�L�X�g</param>
	void Draw(RenderContext& renderContext,EnRenderMode renderMode = enRenderMode_Normal,int shadowMapNumber = 0);
	/// <summary>
	/// ���[���h�s����擾�B
	/// </summary>
	/// <returns></returns>
	const Matrix& GetWorldMatrix() const
	{
		return m_world;
	}
	//���擾
	Skeleton& GetSkeleton()
	{
		return m_skeleton;
	}
	/// <summary>
	/// �V���h�E���V�[�o�[�̃t���O��ݒ�
	/// </summary>
	/// <param name="isFlag"></param>
	void SetShadowReciever(bool isFlag)
	{
		m_meshParts.SetShadowReciever(isFlag);
	}
	/// <summary>
	/// �V���h�E���V�[�o�[���ǂ������擾
	/// </summary>
	/// <returns></returns>
	bool GetShadowReciever()
	{
		return m_meshParts.GetShadowReciever();
	}
	/// <summary>
	/// �V���h�E�L���X�^�[�̃t���O��ݒ�
	/// </summary>
	/// <param name="isFlag"></param>
	void SetShadowCaster(bool isFlag)
	{
		m_isShadowCaster = isFlag;
	}
	/// <summary>
	/// �V���h�E�L���X�^�[���ǂ������擾
	/// </summary>
	/// <returns></returns>
	bool GetShadowCaster()
	{
		return m_isShadowCaster;
	}
private:
	void InitSkeleton(const char* filePath);

private:
	Matrix m_world;			//���[���h�s��B
	TkmFile m_tkmFile;		//tkm�t�@�C���B
	Skeleton m_skeleton;	//�X�P���g���B
	MeshParts m_meshParts;	//���b�V���p�[�c�B
	bool m_isShadowCaster = true;
};
