#pragma once

class ModelRender
{
public:
	ModelRender();
	~ModelRender();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="filePath">モデルのファイルパス(tkm)</param>
	/// <param name="animationClips">アニメーションクリップ</param>
	/// <param name="numAnimationClips">アニメーションクリップの数</param>
	void Init(const char* filePath,
		AnimationClip* animationClips = nullptr,
		int numAnimationClips = 0
	);
	//アニメーションの初期化
	void InitAnimation(AnimationClip* animationClips, int numAnimationClips);
	//座標を設定
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	//回転を設定
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	//大きさを設定
	void SetRotation(const Vector3& scale)
	{
		m_scale = scale;
	}
	/*!
	* @brief	アニメーションを再生。
	*@param[in]	clipNo	アニメーションクリップの番号。コンストラクタに渡したanimClipListの並びとなる。
	*@param[in]	interpolateTime		補完時間(単位：秒)
	*/
	void PlayAnimation(int animNo, float interpolateTime = 0.0f)
	{
		m_animation.Play(animNo, interpolateTime);
	}
	//ワールド行列を更新
	void Update();
	//モデルをドローする
	void Draw();
private:
	Model m_model;										//モデル
	Animation m_animation;								//アニメーション
	AnimationClip* m_animationClip;						//アニメーションクリップ
	int m_numAnimationClips = 0;						//アニメーションクリップの数
	Vector3 m_position = Vector3::Zero;					//座標
	Vector3 m_scale = Vector3::One;						//大きさ
	Quaternion m_rotation = Quaternion::Identity;		//回転
	Matrix m_worldMatrix;								//ワールド行列
	bool m_isInitAnimation = false;						//アニメーション初期化した？
	const char* m_filePath = nullptr;

};

