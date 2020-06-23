#include "stdafx.h"
#include "Model.h"


void Model::Init(const ModelInitData& initData)
{

	//�����̃V�F�[�_�[�����[�h���鏈�������߂Ă���̂�
	//wchar_t�^�̕�����Ȃ̂ŁA�����ŕϊ����Ă����B
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
	//�X�P���g���̃f�[�^��ǂݍ��ށB
	//cmo�t�@�C���̊g���q��tks�ɕύX����B
	std::string skeletonFilePath = filePath;
	//�����񂩂�.cmo�t�@�C���n�܂�ꏊ�������B
	int pos = (int)skeletonFilePath.find(".tkm");
	//.cmo�t�@�C����.tks�ɒu��������B
	skeletonFilePath.replace(pos, 4, ".tks");
	//tks�t�@�C�������[�h����B
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
	//�X�P���g���ɓn���s��̓o�C�A�X����
	m_skeleton.Update(mScale * mRot * mTrans);
}
void Model::Draw(RenderContext& rc, EnRenderMode renderMode)
{
	m_meshParts.Draw(
		rc,
		m_world,
		g_camera3D->GetViewMatrix(),
		g_camera3D->GetProjectionMatrix(),
		renderMode
	);
}
