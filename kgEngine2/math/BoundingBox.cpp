#include "stdafx.h"
#include "BoundingBox.h"

BoundingBox::BoundingBox()
{

}

BoundingBox::~BoundingBox()
{

}

void BoundingBox::Init(const Vector3& halfSize)
{
	//�X�L�����f���̃��[���h�s���90�x�̉�]���������Ă邩��y��z�̍��W���t�ɂ���
	m_halfSize = halfSize;
	m_halfSize.y = halfSize.z;
	m_halfSize.z = halfSize.y;
}

void BoundingBox::Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale)
{
	Matrix worldMatrix, transMatrix, rotMatrix, scaleMatrix;
	//���s�ړ��s����쐬����B
	transMatrix.MakeTranslation(pos);
	//��]�s����쐬����B
	rotMatrix.MakeRotationFromQuaternion(rot);
	//�g��s����쐬����B
	scaleMatrix.MakeScaling(Vector3::One);
	//���[���h�s����쐬����B
	//�g��~��]�~���s�ړ��̏��Ԃŏ�Z����悤�ɁI
	//���Ԃ��ԈႦ���猋�ʂ��ς���B
	worldMatrix.Multiply(scaleMatrix, rotMatrix);
	worldMatrix.Multiply(worldMatrix, transMatrix);
	Update(worldMatrix);
}

void BoundingBox::Update(const Matrix& worldMatrix)
{
	//8���_�̍��W���v�Z�B
	m_vertexPosition[0] = Vector3::Zero;
	m_vertexPosition[0].x -= m_halfSize.x;
	m_vertexPosition[0].y -= m_halfSize.y;
	m_vertexPosition[0].z -= m_halfSize.z;

	m_vertexPosition[1] = Vector3::Zero;
	m_vertexPosition[1].x += m_halfSize.x;
	m_vertexPosition[1].y -= m_halfSize.y;
	m_vertexPosition[1].z -= m_halfSize.z;

	m_vertexPosition[2] = Vector3::Zero;
	m_vertexPosition[2].x -= m_halfSize.x;
	m_vertexPosition[2].y += m_halfSize.y;
	m_vertexPosition[2].z -= m_halfSize.z;

	m_vertexPosition[3] = Vector3::Zero;
	m_vertexPosition[3].x += m_halfSize.x;
	m_vertexPosition[3].y += m_halfSize.y;
	m_vertexPosition[3].z -= m_halfSize.z;

	m_vertexPosition[4] = Vector3::Zero;
	m_vertexPosition[4].x -= m_halfSize.x;
	m_vertexPosition[4].y -= m_halfSize.y;
	m_vertexPosition[4].z += m_halfSize.z;

	m_vertexPosition[5] = Vector3::Zero;
	m_vertexPosition[5].x += m_halfSize.x;
	m_vertexPosition[5].y -= m_halfSize.y;
	m_vertexPosition[5].z += m_halfSize.z;

	m_vertexPosition[6] = Vector3::Zero;
	m_vertexPosition[6].x -= m_halfSize.x;
	m_vertexPosition[6].y += m_halfSize.y;
	m_vertexPosition[6].z += m_halfSize.z;

	m_vertexPosition[7] = Vector3::Zero;
	m_vertexPosition[7].x += m_halfSize.x;
	m_vertexPosition[7].y += m_halfSize.y;
	m_vertexPosition[7].z += m_halfSize.z;

	//��]�����Z
	for (auto& pos : m_vertexPosition) {
		worldMatrix.Apply(pos);
	}
	//������ӂ����[���h�s��̉�]�̊֌W�ł�����ƕς��Ă���
	//X
	//�E�̖�
	m_vertexPosition2[0] = (m_vertexPosition[1] + m_vertexPosition[3] + m_vertexPosition[5] + m_vertexPosition[7]) / 4;

	//���̖�
	m_vertexPosition2[1] = (m_vertexPosition[0] + m_vertexPosition[2] + m_vertexPosition[4] + m_vertexPosition[6]) / 4;

	//Y
	//��ԏ�̖�
	m_vertexPosition2[2] = (m_vertexPosition[4] + m_vertexPosition[5] + m_vertexPosition[6] + m_vertexPosition[7]) / 4;

	//��ԉ��̖�
	m_vertexPosition2[3] = (m_vertexPosition[0] + m_vertexPosition[1] + m_vertexPosition[2] + m_vertexPosition[3]) / 4;

	//Z
	//���̖�
	m_vertexPosition2[4] = (m_vertexPosition[2] + m_vertexPosition[3] + m_vertexPosition[6] + m_vertexPosition[7]) / 4;

	//��O�̖�
	m_vertexPosition2[5] = (m_vertexPosition[0] + m_vertexPosition[1] + m_vertexPosition[4] + m_vertexPosition[5]) / 4;
}

bool BoundingBox::GetisLowPositionY(const Vector3& position, Enxyz xyz) const
{
	switch (xyz) {
	case enState_X:
		if (position.y <= m_vertexPosition2[0].y && position.y <= m_vertexPosition2[1].y) {
			return true;
		}
		break;
	case enState_Y:
		if (position.y <= m_vertexPosition2[2].y && position.y <= m_vertexPosition2[3].y) {
			return true;
		}
		break;
	case enState_Z:
		if (position.y <= m_vertexPosition2[4].y && position.y <= m_vertexPosition2[5].y) {
			return true;
		}
		break;
	}
	return false;
}

Vector3 BoundingBox::SurfaceLineSegment(Enxyz xyz) const
{
	switch (xyz) {
	case enState_X:
		if (m_vertexPosition2[0].y >= m_vertexPosition2[1].y) {
			return m_vertexPosition2[1];
		}
		else {
			return m_vertexPosition2[0];
		}
		break;
	case enState_Y:
		if (m_vertexPosition2[2].y >= m_vertexPosition2[3].y) {
			return m_vertexPosition2[3];
		}
		else {
			return m_vertexPosition2[2];
		}
		break;
	case enState_Z:
		if (m_vertexPosition2[4].y >= m_vertexPosition2[5].y) {
			return m_vertexPosition2[5];
		}
		else {
			return m_vertexPosition2[4];
		}
		break;
	}

}