

//モデル用の定数バッファ
cbuffer ModelCb : register(b0) {
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
	float3	eyesssPos;
	float	specPow;
};


//頂点シェーダーへの入力。
struct SVSIn {
	float4 pos 		: POSITION;		//モデルの頂点座標。
	float3 normal	: NORMAL;		//法線。
	float2 uv 		: TEXCOORD0;	//UV座標。
	float3 tangent	: TANGENT;
	uint4  indices  : BLENDINDICES0;
	float4 weights  : BLENDWEIGHT0;
};


/*struct SVSIn {
	float4 pos : SV_Position;
	float3 normal   : NORMAL;
	float2 uv	: TEXCOORD0;
	float3 tangent	: TANGENT;
	//uint4  indices  : BLENDINDICES0;
	//float4 weights  : BLENDWEIGHT0;
}*/

//ボーン行列
StructuredBuffer<float4x4> boneMatrix : register(t3);
//StructuredBuffer boneMatrix : register(t1);

//ピクセルシェーダーへの入力。
struct SPSIn {
	float4 pos 			: SV_POSITION;	//スクリーン空間でのピクセルの座標。
	float3 normal		: NORMAL;		//法線。
	float2 uv 			: TEXCOORD0;	//uv座標。
	float3 worldPos		: TEXCOORD1;	//ワールド空間でのピクセルの座標。
	float3 eyePos		: TEXCOORD2;
	float  specPow : TEXCOORD3;
};

//ピクセルシェーダーからの出力
struct SPSOut {
	float4 albedo : SV_Target0;		//アルベド
	float3 normal : SV_Target1;		//法線
};

//モデルテクスチャ。
Texture2D<float4> g_texture : register(t0);
//法線マップ
Texture2D<float4> g_normal : register(t1);
//スペキュラマップ
Texture2D<float4> g_specular : register(t2);

//サンプラステート。
sampler g_sampler : register(s0);


/*!
 *@brief	スキン行列を計算。
 */
float4x4 CalcSkinMatrix(SVSIn vsIn)
{
	float4x4 skinning = 0;
	float w = 0.0f;
	[unroll]
	for (int i = 0; i < 3; i++)
	{
		skinning += boneMatrix[vsIn.indices[i]] *vsIn.weights[i];
		w += vsIn.weights[i];
	}

	skinning += boneMatrix[vsIn.indices[3]] * (1.0f - w);
	return skinning;
}

/// <summary>
/// モデル用の頂点シェーダーのエントリーポイント。
/// </summary>
SPSIn VSMain(SVSIn vsIn, uniform bool hasSkin)
{
	SPSIn psIn;

	psIn.pos = mul(mWorld, vsIn.pos);			//モデルの頂点をワールド座標系に変換。
	psIn.worldPos = psIn.pos.xyz;
	psIn.pos = mul(mView, psIn.pos);			//ワールド座標系からカメラ座標系に変換。
	psIn.pos = mul(mProj, psIn.pos);			//カメラ座標系からスクリーン座標系に変換。
	psIn.normal = mul(mWorld, vsIn.normal);		//法線をワールド座標系に変換。
	psIn.uv = vsIn.uv;
	//psIn.eyePos = vsIn.eyesssPos;
	//psIn.specPow = vsIn.specPow;

	return psIn;
}

/// <summary>
/// スキンありモデル用の頂点シェーダーのエントリーポイント。
/// </summary>
SPSIn VSMainSkin(SVSIn vsIn)
{
	SPSIn psIn;

	float4x4 skinning = CalcSkinMatrix(vsIn);
	psIn.pos = mul(skinning, vsIn.pos);
	psIn.worldPos = psIn.pos.xyz;
	psIn.pos = mul(mView, psIn.pos);			//ワールド座標系からカメラ座標系に変換。
	psIn.pos = mul(mProj, psIn.pos);			//カメラ座標系からスクリーン座標系に変換。
	psIn.normal = mul(mWorld, vsIn.normal);		//法線をワールド座標系に変換。
	psIn.uv = vsIn.uv;
	//psIn.eyePos = vsIn.eyesssPos;
	//psIn.specPow = vsIn.specPow;

	return psIn;
}

/// <summary>
/// モデル用のピクセルシェーダーのエントリーポイント
/// </summary>
SPSOut PSMain(SPSIn psIn)
{
	float4 color = g_texture.Sample(g_sampler,psIn.uv);

	////拡散反射光////
	/*float3 lig = float3(0.0f, 0.0f, 0.0f);
	lig = max(0.0f, dot(psIn.normal * -1.0f, directionR)) * colorR;
	lig += max(0.0f, dot(psIn.normal * -1.0f, directionG)) * colorG;
	lig += max(0.0f, dot(psIn.normal * -1.0f, directionB)) * colorB;

	////鏡面反射////
	//1 反射ベクトルRを求める。
	float3 R = 0.f;
	R += float3(0.0f, 0.0f, -1.0f) + -2 * dot(psIn.normal, -1 * float3(0.0f, 0.0f, -1.0f)) * psIn.normal;

	//2 視点からライトを当てる物体に伸びるVectorEを求める
	float3 E = normalize(psIn.worldPos - psIn.eyePos);

	//1と2で求まったVectorのない席を計算する。
	//スペキュラ反射の強さを求める。
	float specPower = max(0, dot(R, -E));

	//3 スペキュラ反射をライトに加算する。
	lig += colorR.xyz * pow(specPower, psIn.specPow);
	lig += colorG.xyz * pow(specPower, psIn.specPow);
	lig += colorB.xyz * pow(specPower, psIn.specPow);*/

	////最終カラーの決定////
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//finalColor.xyz = color.xyz * lig;
	finalColor.xyz = color.xyz;

	SPSOut spsOut;
	spsOut.albedo = finalColor;
	spsOut.normal = psIn.normal;

	return spsOut;
	//return float4(specPow, 0.0f, 0.0f, 1.0f);    
}
