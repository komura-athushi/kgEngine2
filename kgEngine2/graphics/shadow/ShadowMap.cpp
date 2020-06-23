#include "stdafx.h"
#include "ShadowMap.h"
#include "graphics/Model.h"

ShadowMap::ShadowMap()
{
	/*m_shadowMapRT.Create(
		4096,
		4096,
		1,
		1,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_D32_FLOAT
	);*/

	m_shadowMapRT.Create(
		FRAME_BUFFER_W,
		FRAME_BUFFER_H,
		1,
		1,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_D32_FLOAT
	);
}

void ShadowMap::UpdateFromLightDirection(const Vector3& lightCameraPos, const Vector3& lightDir)
{
	//���C�g�̕����ɂ���āA���C�g�J�����̏���������߂�
	Vector3 lightCameraUpAxis;

	if (fabsf(lightDir.y) > 0.999998f) {
		//�قڐ^��or�^���������Ă���̂ŁA1,0,0��������Ƃ���
		lightCameraUpAxis = Vector3::AxisX;
	}
	else {
		//����ȊO�̂Ƃ��́A0,1,0��������Ƃ���
		lightCameraUpAxis = Vector3::AxisY;
	}
	m_lightViewMatrix.MakeLookAt(
		m_lightCameraPosition,
		m_lightCamerataraget,
		lightCameraUpAxis
	);

	m_lightProjMatrix.MakeOrthoProjectionMatrix(
		300,
		300,
		10.0f,
		1500.0f
	);

}

void ShadowMap::UpdateFromLightTaraget(const Vector3& lightCameraPos,const Vector3& lightCameraTarget)
{
	m_lightCameraPosition = lightCameraPos;
	m_lightCamerataraget = lightCameraTarget;
	//���C�g�̕������v�Z����
	auto lightDir = m_lightCamerataraget - m_lightCameraPosition;
	if (lightDir.Length() < 0.00001f) {
		//���C�g�J�����̍��W�ƒ����_���߂�����̂ŃN���b�V��������
		std::abort();
	}

	//���K�����āA�����x�N�g���ɕϊ�����
	lightDir.Normalize();

	UpdateFromLightDirection(lightCameraPos, lightDir);
}

void ShadowMap::RenderToShadowMap(RenderContext& rc)
{
	RenderTarget* rt[] = {
		&m_shadowMapRT
	};
	//�V���h�E�}�b�v�̃����_�����O�^�[�Q�b�g��ҋ@������Ԃɂ���
	rc.WaitUntilToPossibleSetRenderTargets(1, rt);
	rc.SetRenderTargetAndViewport(m_shadowMapRT);
	const float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	rc.ClearRenderTargetView(m_shadowMapRT, clearColor);

	if (m_shadowCaters.size() >= 1) {
		for (auto caster : m_shadowCaters) {
			caster->Draw(g_graphicsEngine->GetRenderContext(),enRenderMode_CreateShadowMap);
		}
	}

	m_shadowCaters.clear();

	g_graphicsEngine->ChangeRenderTargetToFrameBuffer(g_graphicsEngine->GetRenderContext());
}