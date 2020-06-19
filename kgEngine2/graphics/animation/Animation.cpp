/*!
 * @brief	�A�j���[�^�[�N���X�B
 */


#include "stdafx.h"
#include "graphics/animation/Animation.h"
#include "graphics/skeleton.h"
#include "graphics/Model.h"

Animation::Animation()
{
}
Animation::~Animation()
{
	
}
	
void Animation::Init(Model& Model, AnimationClip animClipList[], int numAnimClip)
{
	if (animClipList == nullptr) {
#ifdef _DEBUG
		char message[256];
		strcpy(message, "animClipList��NULL�ł��B\n");
		OutputDebugStringA(message);
		//�~�߂�B
		std::abort();
#endif
		
	}
	m_skeleton = &Model.GetSkeleton();

	for (int i = 0; i < numAnimClip; i++) {
		m_animationClips.push_back(&animClipList[i]);
	}
	for (auto& ctr : m_animationPlayController) {
		ctr.Init(m_skeleton);
	}
		
	Play(0);
}
/*!
* @brief	���[�J���|�[�Y�̍X�V�B
*/
void Animation::UpdateLocalPose(float deltaTime)
{
	m_interpolateTime += deltaTime;
	if (m_interpolateTime >= 1.0f) {
		//��Ԋ����B
		//���݂̍ŏI�A�j���[�V�����R���g���[���ւ̃C���f�b�N�X���J�n�C���f�b�N�X�ɂȂ�B
		m_startAnimationPlayController = GetLastAnimationControllerIndex();
		m_numAnimationPlayController = 1;
		m_interpolateTime = 1.0f;
	}
	//AnimationPlayController::Update�֐������s���Ă����B
	for (int i = 0; i < m_numAnimationPlayController; i++) {
		int index = GetAnimationControllerIndex(m_startAnimationPlayController, i );
		m_animationPlayController[index].Update(deltaTime, this);
	}
}

void Animation::UpdateGlobalPose()
{
	//�O���[�o���|�[�Y�v�Z�p�̃��������X�^�b�N����m�ہB
	int numBone = m_skeleton->GetNumBones();
	Quaternion* qGlobalPose = (Quaternion*)alloca(sizeof(Quaternion) * numBone);
	Vector3* vGlobalPose = (Vector3*)alloca(sizeof(Vector3) * numBone);
	Vector3* vGlobalScale = (Vector3*)alloca(sizeof(Vector3) * numBone);
	for (int i = 0; i < numBone; i++) {
		qGlobalPose[i] = Quaternion::Identity;
		vGlobalPose[i] = Vector3::Zero;
		vGlobalScale[i] = Vector3::One;
	}
	//�O���[�o���|�[�Y���v�Z���Ă����B
	int startIndex = m_startAnimationPlayController;
	for (int i = 0; i < m_numAnimationPlayController; i++) {
		int index = GetAnimationControllerIndex(startIndex, i);
		float intepolateRate = m_animationPlayController[index].GetInterpolateRate();
		const auto& localBoneMatrix = m_animationPlayController[index].GetBoneLocalMatrix();
		for (int boneNo = 0; boneNo < numBone; boneNo++) {
			//���s�ړ��̕⊮
			Matrix m = localBoneMatrix[boneNo];
			vGlobalPose[boneNo].Lerp(
				intepolateRate, 
				vGlobalPose[boneNo], 
				*(Vector3*)m.m[3]
			);
			//���s�ړ��������폜�B
			m.m[3][0] = 0.0f;
			m.m[3][1] = 0.0f;
			m.m[3][2] = 0.0f;
			
			//�g�听���̕�ԁB
			Vector3 vBoneScale;
			vBoneScale.x = (*(Vector3*)m.m[0]).Length();
			vBoneScale.y = (*(Vector3*)m.m[1]).Length();
			vBoneScale.z = (*(Vector3*)m.m[2]).Length();

			vGlobalScale[boneNo].Lerp(
				intepolateRate,
				vGlobalScale[boneNo],
				vBoneScale
			);
			//�g�听���������B
			m.m[0][0] /= vBoneScale.x;
			m.m[0][1] /= vBoneScale.x;
			m.m[0][2] /= vBoneScale.x;

			m.m[1][0] /= vBoneScale.y;
			m.m[1][1] /= vBoneScale.y;
			m.m[1][2] /= vBoneScale.y;

			m.m[2][0] /= vBoneScale.z;
			m.m[2][1] /= vBoneScale.z;
			m.m[2][2] /= vBoneScale.z;
				
			//��]�̕⊮
			Quaternion qBone;
			qBone.SetRotation(m);
			qGlobalPose[boneNo].Slerp(intepolateRate, qGlobalPose[boneNo], qBone);
		}
	}
	//�O���[�o���|�[�Y���X�P���g���ɔ��f�����Ă����B
	for (int boneNo = 0; boneNo < numBone; boneNo++) {
		
		//�g��s����쐬�B
		Matrix scaleMatrix;
		scaleMatrix.MakeScaling(vGlobalScale[boneNo]);
		//��]�s����쐬�B
		Matrix rotMatrix;
		rotMatrix.MakeRotationFromQuaternion(qGlobalPose[boneNo]);
		//���s�ړ��s����쐬�B
		Matrix transMat;
		transMat.MakeTranslation(vGlobalPose[boneNo]);

		//�S�����������āA�{�[���s����쐬�B
		Matrix boneMatrix;
		boneMatrix.Multiply(scaleMatrix, rotMatrix);
		boneMatrix.Multiply(boneMatrix, transMat);
		
		m_skeleton->SetBoneLocalMatrix(
			boneNo,
			boneMatrix
		);			
	}
		
	//�ŏI�A�j���[�V�����ȊO�͕�Ԋ������Ă����珜�����Ă����B
	int numAnimationPlayController = m_numAnimationPlayController;
	for (int i = 1; i < m_numAnimationPlayController; i++) {
		int index = GetAnimationControllerIndex(startIndex, i);
		if (m_animationPlayController[index].GetInterpolateRate() > 0.99999f) {
			//��Ԃ��I����Ă���̂ŃA�j���[�V�����̊J�n�ʒu��O�ɂ���B
			m_startAnimationPlayController = index;
			numAnimationPlayController = m_numAnimationPlayController - i;
		}
	}
	m_numAnimationPlayController = numAnimationPlayController;
}
	

	
void Animation::Update(float deltaTime)
{
	if (m_numAnimationPlayController == 0) {
		return;
	}
	//���[�J���|�[�Y�̍X�V������Ă����B
	UpdateLocalPose(deltaTime);
		
	//�O���[�o���|�[�Y���v�Z���Ă����B
	UpdateGlobalPose();

	m_skeleton->SetPlayAnimation();
}