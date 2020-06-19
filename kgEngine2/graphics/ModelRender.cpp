#include "stdafx.h"
#include "ModelRender.h"


ModelRender::ModelRender()
{

}

ModelRender::~ModelRender()
{

}

void ModelRender::Init(const char* filePath,
	AnimationClip* animationClips,
	int numAnimationClips
)
{
	m_filePath = filePath;

	//���f���̏�����������
	ModelInitData initData;
	initData.m_tkmFilePath = filePath;
	//�V�F�[�_�[�͍���Ƃ�model.fx�ŌŒ肷��̂ł�
	initData.m_fxFilePath = "Assets/shader/model.fx";
	m_model.Init(initData);

	//�A�j���[�V�����̏�����
	InitAnimation(animationClips, numAnimationClips);
}

void ModelRender::InitAnimation(AnimationClip* animationClips, int numAnimationClips)
{
	//�A�j���[�V�����̏�����
	if (m_isInitAnimation) {
		return;
	}

	//�A�j���[�V�����N���b�v���ݒ肳��Ă���
	if (animationClips != nullptr) {
		m_animationClip = animationClips;
		m_numAnimationClips = numAnimationClips;
		m_animation.Init(m_model, m_animationClip, m_numAnimationClips);
		m_model.BindSkeleton();
		m_isInitAnimation = true;
	}

}

void ModelRender::Update()
{
	//���f���̃��[���h�s����X�V
	m_model.UpdateWorldMatrix(m_position,m_rotation,m_scale);
	m_animation.Update(1.0f / 60.0f);
}

void ModelRender::Draw()
{
	auto& renderContext = g_graphicsEngine->GetRenderContext();
	//���f�����h���[
	m_model.Draw(renderContext);
}
