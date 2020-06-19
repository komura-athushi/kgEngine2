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

	//モデルの初期化すっぺ
	ModelInitData initData;
	initData.m_tkmFilePath = filePath;
	//シェーダーは今んとこmodel.fxで固定するのです
	initData.m_fxFilePath = "Assets/shader/model.fx";
	m_model.Init(initData);

	//アニメーションの初期化
	InitAnimation(animationClips, numAnimationClips);
}

void ModelRender::InitAnimation(AnimationClip* animationClips, int numAnimationClips)
{
	//アニメーションの初期化
	if (m_isInitAnimation) {
		return;
	}

	//アニメーションクリップが設定されてたら
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
	//モデルのワールド行列を更新
	m_model.UpdateWorldMatrix(m_position,m_rotation,m_scale);
	m_animation.Update(1.0f / 60.0f);
}

void ModelRender::Draw()
{
	auto& renderContext = g_graphicsEngine->GetRenderContext();
	//モデルをドロー
	m_model.Draw(renderContext);
}
