#pragma once

enum Enxyz {
	enState_X,
	enState_Y,
	enState_Z,
	enState_No
};

//�o�E���f�B���O�{�b�N�X
class BoundingBox {
public:
	BoundingBox();
	~BoundingBox();
	//������
	void Init(const Vector3& halfSize);
	//void Init(const float min)
	//�X�V
	void Update(const Matrix& worldMatrix);
	void Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale = Vector3::One);
	//�Y���̖ʂ̒��S��y���W���v���C���[�̍��W���Ⴂ���ǂ���
	bool GetisLowPositionY(const Vector3& position, Enxyz xyz) const;
	//�����Ɏg�����߂̊Y���̖ʂ̒��S�̍��W���擾
	Vector3 SurfaceLineSegment(Enxyz xyz) const;
	//�Y���̖ʂ̒��S���W���擾
	Vector3 GetSurfaceVector(const int& number) const
	{
		return m_vertexPosition2[number];
	}
	//���_�̍��W���擾
	Vector3 GetVertexVector(const int number) const
	{
		return m_vertexPosition[number];
	}
	//�z��̑傫��
	static const int m_SurfaceVectorNumber = 6;
	static const int m_vertexNumber = 8;
private:
	Vector3 m_halfSize = Vector3::Zero;					//�x�N�g���̊e�v�f�̍ő�l�̔����̒l
	Vector3 m_vertexPosition[8];							//�o�E���f�B���O�{�b�N�X�̊e���_���W
	Vector3 m_vertexPosition2[6];							//�e�ʂ̒��S���W
};

