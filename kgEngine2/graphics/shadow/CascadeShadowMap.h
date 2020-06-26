#pragma once
#include "graphics/RenderTarget.h"
#include "graphics/RenderMode.h"


class Model;
class CascadeShadowMap
{
public:
	CascadeShadowMap();
	/// <summary>
	/// ライトビュー行列を取得
	/// </summary>
	/// <returns></returns>
	Matrix GetLightViewMatrix()
	{
		return m_lightViewMatrix;
	}
	/// <summary>
	/// ライトプロジェクション行列を取得
	/// </summary>
	/// <returns></returns>
	Matrix GetLightProjMatrix()
	{
		return m_lightProjMatrix;
	}
	/// <summary>
	/// ライトビュー行列やライトプロジェクション行列を更新
	/// </summary>
	/// <param name="lightCameraPos"></param>
	/// <param name="lightCameraTarget"></param>
	void UpdateFromLightTaraget(const Vector3& lightCameraPos, const Vector3& lightCameraTarget);
	/// <summary>
	/// 更新(ライトカメラの向きを指定するバージョン)
	/// </summary>
	/// <param name="lightCameraPos"></param>
	/// <param name="lightDir"></param>
	void UpdateFromLightDirection(const Vector3& lightCameraPos, const Vector3& lightDir);
	/// <summary>
	/// シャドウマップにシャドウキャスターをレンダリング
	/// </summary>
	void RenderToShadowMap(RenderContext& rc);
	/// <summary>
	/// シャドウキャスターを追加
	/// </summary>
	/// <param name="shadowCaster"></param>
	void RegistShadowCaster(Model* shadowCaster)
	{
		m_shadowCaters.push_back(shadowCaster);
	}
	/// <summary>
	/// シャドウマップのテクスチャを取得
	/// </summary>
	/// <returns></returns>
	Texture& GetShadowMapTexture(int number)
	{
		return m_shadowMapRT[number].GetRenderTargetTexture();
	}
	/// <summary>
	/// レンダリングターゲットを取得
	/// </summary>
	/// <returns></returns>
	RenderTarget* GetRenderTarget(int number)
	{
		return &m_shadowMapRT[number];
	}
	static const int SHADOWMAP_NUM = 4;
private:
	Vector3 m_lightCameraPosition = Vector3(300.0f, 300.0f, -300.0f);		//ライトカメラの座標	
	Vector3 m_lightCamerataraget = Vector3(0.0f, 0.0f, 0.0f);		//ライトカメラの注視点
	Matrix m_lightViewMatrix;			//ライトビュー行列
	Matrix m_lightProjMatrix;			//ライトプロジェクション行列
	RenderTarget m_shadowMapRT[SHADOWMAP_NUM];			//シャドウマップのレンダリングターゲット
	std::vector<Model*> m_shadowCaters;		//シャドウキャスターのリスト
};


