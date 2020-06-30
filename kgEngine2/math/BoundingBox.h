#pragma once

enum Enxyz {
	enState_X,
	enState_Y,
	enState_Z,
	enState_No
};

//バウンディングボックス
class BoundingBox {
public:
	BoundingBox();
	~BoundingBox();
	//初期化
	void Init(const Vector3& halfSize);
	//void Init(const float min)
	//更新
	void Update(const Matrix& worldMatrix);
	void Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale = Vector3::One);
	//該当の面の中心のy座標がプレイヤーの座標より低いかどうか
	bool GetisLowPositionY(const Vector3& position, Enxyz xyz) const;
	//線分に使うための該当の面の中心の座標を取得
	Vector3 SurfaceLineSegment(Enxyz xyz) const;
	//該当の面の中心座標を取得
	Vector3 GetSurfaceVector(const int& number) const
	{
		return m_vertexPosition2[number];
	}
	//頂点の座標を取得
	Vector3 GetVertexVector(const int number) const
	{
		return m_vertexPosition[number];
	}
	//配列の大きさ
	static const int m_SurfaceVectorNumber = 6;
	static const int m_vertexNumber = 8;
private:
	Vector3 m_halfSize = Vector3::Zero;					//ベクトルの各要素の最大値の半分の値
	Vector3 m_vertexPosition[8];							//バウンディングボックスの各頂点座標
	Vector3 m_vertexPosition2[6];							//各面の中心座標
};

