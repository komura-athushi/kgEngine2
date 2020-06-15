

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
};
//ピクセルシェーダーへの入力。
struct SPSIn {
	float4 pos 			: SV_POSITION;	//スクリーン空間でのピクセルの座標。
	float3 normal		: NORMAL;		//法線。
	float2 uv 			: TEXCOORD0;	//uv座標。
	float3 worldPos		: TEXCOORD1;	//ワールド空間でのピクセルの座標。
	float3 eyePos		: TEXCOORD2;
	float  specPow : TEXCOORD3;
};

//モデルテクスチャ。
Texture2D<float4> g_texture : register(t0);

//サンプラステート。
sampler g_sampler : register(s0);


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
/// モデル用のピクセルシェーダーのエントリーポイント
/// </summary>
float4 PSMain(SPSIn psIn) : SV_Target0
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
	return finalColor;
	//return float4(specPow, 0.0f, 0.0f, 1.0f);    
}
