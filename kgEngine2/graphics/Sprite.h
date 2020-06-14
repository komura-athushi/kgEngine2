#pragma once

#include "Indexbuffer.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"

class Texture;

/// <summary>
/// �X�v���C�g�N���X�B
/// </summary>
class Sprite  {
public:
	static const Vector2	DEFAULT_PIVOT;					//!<�s�{�b�g�B
	virtual ~Sprite();
	/// <summary>
	/// �������B
	/// </summary>
	/// <param name="ddsFilePath">dds�t�@�C���̃t�@�C���p�X</param>
	/// <param name="w">��</param>
	/// <param name="h">����</param>
	void Init(const char* ddsFilePath, float w, float h);
	/// <summary>
	/// �X�V�B
	/// </summary>
	/// <param name="pos">���W</param>
	/// <param name="rot">��]</param>
	/// <param name="scale">�g�嗦</param>
	/// <param name="pivot">
	/// �s�{�b�g
	/// 0.5, 0.5�ŉ摜�̒��S����_�B
	/// 0.0, 0.0�ŉ摜�̍����B
	/// 1.0, 1.0�ŉ摜�̉E��B
	/// Unity��uGUI�ɏ����B
	/// </param>
	void Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale, const Vector2& pivot = DEFAULT_PIVOT);
	/// <summary>
	/// �`��B
	/// </summary>
	/// <param name="renderContext">�����_�����O�R���e�L�X�g/param>
	void Draw(RenderContext& renderContext);
private:
	IndexBuffer m_indexBuffer;		//�C���f�b�N�X�o�b�t�@�B
	VertexBuffer m_vertexBuffer;	//���_�o�b�t�@�B
	Texture m_texture;				//�e�N�X�`���B
	Vector3 m_position ;			//���W�B
	Vector2 m_size;					//�T�C�Y�B
	Quaternion m_rotation ;			//��]�B
	Matrix m_world;					//���[���h�s��B

	struct LocalConstantBuffer {
		Matrix mvp;
		Vector4 mulColor;
	};
	LocalConstantBuffer m_constantBufferCPU;	//CPU���̒萔�o�b�t�@�B
	ConstantBuffer		m_constantBufferGPU;	//GPU���̒萔�o�b�t�@�B
	DescriptorHeap		m_descriptorHeap;		//�f�B�X�N���v�^�q�[�v�B
	RootSignature		m_rootSignature;		//���[�g�V�O�l�`���B
	PipelineState		m_pipelineState;		//�p�C�v���C���X�e�[�g�B
	Shader				m_vs;					//���_�V�F�[�_�[�B
	Shader				m_ps;					//�s�N�Z���V�F�[�_�[�B
};