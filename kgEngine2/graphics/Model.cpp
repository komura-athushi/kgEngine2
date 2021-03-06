#include "stdafx.h"
#include "Model.h"


void Model::Init(const ModelInitData& initData)
{

	//内部のシェーダーをロードする処理が求めているのが
	//wchar_t型の文字列なので、ここで変換しておく。
	wchar_t wfxFilePath[256];
	mbstowcs(wfxFilePath, initData.m_fxFilePath, 256);

	m_tkmFile.Load(initData.m_tkmFilePath);
	m_meshParts.InitFromTkmFile(
		m_tkmFile, 
		wfxFilePath, 
		initData.m_vsEntryPointFunc,
		initData.m_psEntryPointFunc,
		initData.m_expandConstantBuffer,
		initData.m_expandConstantBufferSize
	);

	InitSkeleton(initData.m_tkmFilePath);

	UpdateWorldMatrix(g_vec3Zero, g_quatIdentity, g_vec3One);
}

void Model::InitSkeleton(const char* filePath)
{
	//スケルトンのデータを読み込む。
	//cmoファイルの拡張子をtksに変更する。
	std::string skeletonFilePath = filePath;
	//文字列から.cmoファイル始まる場所を検索。
	int pos = (int)skeletonFilePath.find(".tkm");
	//.cmoファイルを.tksに置き換える。
	skeletonFilePath.replace(pos, 4, ".tks");
	//tksファイルをロードする。
	bool result = m_skeleton.Init(skeletonFilePath.c_str());
}

void Model::UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale)
{
	Matrix mBias;
//todo	if (enUpdateAxis == enFbxUpAxisZ) {
		//Z-up
		mBias.MakeRotationX(Math::PI * -0.5f);
//	}
	Matrix mTrans, mRot, mScale;
	mTrans.MakeTranslation(pos);
	mRot.MakeRotationFromQuaternion(rot);
	mScale.MakeScaling(scale);
	m_world = mBias * mScale * mRot * mTrans;

	//m_skeleton.Update(m_world);
	//スケルトンに渡す行列はバイアス無し
	m_skeleton.Update(mScale * mRot * mTrans);
}
void Model::Draw(RenderContext& rc, EnRenderMode renderMode, int shadowMapNumber)
{
	m_meshParts.Draw(
		rc,
		m_world,
		g_camera3D->GetViewMatrix(),
		g_camera3D->GetProjectionMatrix(),
		renderMode,
		shadowMapNumber
	);
}
