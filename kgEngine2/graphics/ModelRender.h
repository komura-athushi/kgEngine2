#pragma once

class ModelRender
{
public:
	ModelRender();
	~ModelRender();
	/// <summary>
	/// ������
	/// </summary>
	/// <param name="filePath">���f���̃t�@�C���p�X(tkm)</param>
	/// <param name="animationClips">�A�j���[�V�����N���b�v</param>
	/// <param name="numAnimationClips">�A�j���[�V�����N���b�v�̐�</param>
	void Init(const char* filePath,
		AnimationClip* animationClips = nullptr,
		int numAnimationClips = 0
	);
	//�A�j���[�V�����̏�����
	void InitAnimation(AnimationClip* animationClips, int numAnimationClips);
	//���W��ݒ�
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	//��]��ݒ�
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	//�傫����ݒ�
	void SetRotation(const Vector3& scale)
	{
		m_scale = scale;
	}
	/*!
	* @brief	�A�j���[�V�������Đ��B
	*@param[in]	clipNo	�A�j���[�V�����N���b�v�̔ԍ��B�R���X�g���N�^�ɓn����animClipList�̕��тƂȂ�B
	*@param[in]	interpolateTime		�⊮����(�P�ʁF�b)
	*/
	void PlayAnimation(int animNo, float interpolateTime = 0.0f)
	{
		m_animation.Play(animNo, interpolateTime);
	}
	//���[���h�s����X�V
	void Update();
	//���f�����h���[����
	void Draw();
private:
	Model m_model;										//���f��
	Animation m_animation;								//�A�j���[�V����
	AnimationClip* m_animationClip;						//�A�j���[�V�����N���b�v
	int m_numAnimationClips = 0;						//�A�j���[�V�����N���b�v�̐�
	Vector3 m_position = Vector3::Zero;					//���W
	Vector3 m_scale = Vector3::One;						//�傫��
	Quaternion m_rotation = Quaternion::Identity;		//��]
	Matrix m_worldMatrix;								//���[���h�s��
	bool m_isInitAnimation = false;						//�A�j���[�V���������������H
	const char* m_filePath = nullptr;

};

