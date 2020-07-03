

static const int NUM_CASCADES = 3;


//モデル用の定数バッファ
cbuffer ModelCb : register(b0) {
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
	float4x4 mLightView;
	float4x4 mLightProj;
	float4x4 mLightViewProj[NUM_CASCADES];
	int isShadowReciever;
	int shadowMapNumber = 0;

};

/// <summary>
/// シャドウマップ用の定数バッファ。
/// </summary>
cbuffer ShadowMapCb : register(b1) {
	float4x4 lightViewProjMatrix;	//ライトビュープロジェクション行列。
}

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


//StructuredBuffer boneMatrix : register(t1);

//ピクセルシェーダーへの入力。
struct SPSIn {
	float4 pos 			: SV_POSITION;	//スクリーン空間でのピクセルの座標。
	float3 normal		: NORMAL;		//法線。
	float2 uv 			: TEXCOORD0;	//uv座標。
	float3 worldPos		: TEXCOORD1;	//ワールド空間でのピクセルの座標。
	float4 posInLVP[NUM_CASCADES]		: TEXCOORD2;	//ライトビュープロジェクション空間での座標
};

/// <summary>
/// シャドウマップ用のピクセルシェーダへの入力構造体。
/// </summary>
struct PSInput_ShadowMap {
	float4 Position 			: SV_POSITION;	//座標。
};

//ピクセルシェーダーからの出力
struct SPSOut {
	float4 albedo : SV_Target0;		//アルベド
	float4 normal : SV_Target1;		//法線
	float shadow : SV_Target2;		//シャドウ
};

//モデルテクスチャ。
Texture2D<float4> g_texture : register(t0);
//法線マップ
Texture2D<float4> g_normal : register(t1);
//スペキュラマップ
Texture2D<float4> g_specularMap : register(t2);
//ボーン行列
StructuredBuffer<float4x4> boneMatrix : register(t3);
//シャドウマップ
Texture2D<float4> g_shadowMap : register(t4);
//カスケードシャドウマップ
Texture2D<float4> g_cascadeShadowMap1 : register(t5);
Texture2D<float4> g_cascadeShadowMap2 : register(t6);
Texture2D<float4> g_cascadeShadowMap3 : register(t7);

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
SPSIn VSMain(SVSIn vsIn)
{
	SPSIn psIn;
	
	float4 worldPos = mul(mWorld, vsIn.pos);			//モデルの頂点をワールド座標系に変換。
	psIn.pos = worldPos;
	psIn.worldPos = psIn.pos.xyz;
	if (isShadowReciever == 1) {
		//ライトビュープロジェクション座標に変換
		for (int i = 0; i < NUM_CASCADES; i++) {
			psIn.posInLVP[i] = mul(mLightViewProj[i], worldPos);
		}
	}
	psIn.pos = mul(mView, psIn.pos);			//ワールド座標系からカメラ座標系に変換。
	psIn.pos = mul(mProj, psIn.pos);			//カメラ座標系からスクリーン座標系に変換。
	psIn.normal = mul(mWorld, vsIn.normal);		//法線をワールド座標系に変換。
	psIn.uv = vsIn.uv;
	

	return psIn;
}

/// <summary>
/// スキンありモデル用の頂点シェーダーのエントリーポイント。
/// </summary>
SPSIn VSMainSkin(SVSIn vsIn)
{
	SPSIn psIn;

	float4x4 skinning = CalcSkinMatrix(vsIn);
	float4 worldPos = mul(skinning, vsIn.pos);
	psIn.pos = worldPos;
	psIn.worldPos = psIn.pos.xyz;
	if (isShadowReciever == 1) {
		//ライトビュープロジェクション座標に変換
		for (int i = 0; i < NUM_CASCADES; i++) {
			psIn.posInLVP[i] = mul(mLightViewProj[i], worldPos);
		}
	}
	psIn.pos = mul(mView, psIn.pos);			//ワールド座標系からカメラ座標系に変換。
	psIn.pos = mul(mProj, psIn.pos);			//カメラ座標系からスクリーン座標系に変換。
	psIn.normal = mul(mWorld, vsIn.normal);		//法線をワールド座標系に変換。
	psIn.uv = vsIn.uv;

	return psIn;
}

/// <summary>
/// モデル用のピクセルシェーダーのエントリーポイント
/// </summary>
SPSOut PSMain(SPSIn psIn) : SV_Target0
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
	spsOut.normal.xyz = psIn.normal;
	spsOut.normal.w = 1.0f;
	
	spsOut.shadow = 1.0f;
	//spsOut.shadow = color.x;
		//シャドウレシーバー。
	if (isShadowReciever == 1) {
		for (int i = 0; i < NUM_CASCADES; i++) {
			//LVP空間から見た時の最も手前の深度値をシャドウマップから取得する。
		//プロジェクション行列をシャドウマップのUV座標に変換している
			float2 shadowMapUV = psIn.posInLVP[i].xy / psIn.posInLVP[i].w;
			shadowMapUV *= float2(0.5f, -0.5f);
			shadowMapUV += 0.5f;
			//spsOut.shadow = shadowMapUV.x;
			//シャドウマップのUV座標範囲内かどうかを判定する。
			if (shadowMapUV.x < 1.0f
				&& shadowMapUV.x > 0.0f
				&& shadowMapUV.y < 1.0f
				&& shadowMapUV.y > 0.0f
				) {
				///LVP空間での深度値を計算。
				float zInLVP = psIn.posInLVP[i].z / psIn.posInLVP[i].w;
				float zInShadowMap;
				//シャドウマップに書き込まれている深度値を取得。
				if (i == 0) {
					zInShadowMap = g_cascadeShadowMap1.Sample(g_sampler, shadowMapUV);
				}
				else if (i == 1) {
					zInShadowMap = g_cascadeShadowMap2.Sample(g_sampler, shadowMapUV);
				}
				else if (i == 2) {
					zInShadowMap = g_cascadeShadowMap3.Sample(g_sampler, shadowMapUV);
				}
				
				if (zInLVP > zInShadowMap + 0.0001f) {
					//影が落ちているので、光を弱くする
					spsOut.shadow = 0.5f;
					//	spsOut.shadow = zInShadowMap;
						//spsOut.shadow = zInLVP;
						//spsOut.shadow = psIn.pos.z / psIn.pos.w;
				
				}
				break;
			}
		}
	}
		

	return spsOut;
}
/// <summary>
/// スキン無しシャドウマップ生成用の頂点シェーダー
/// </summary>
PSInput_ShadowMap VSMain_ShadowMap(SVSIn In)
{
	PSInput_ShadowMap psInput = (PSInput_ShadowMap)0;
	float4 pos = mul(mWorld, In.pos);
	pos = mul(mLightView, pos);
	pos = mul(mLightProj, pos);
	//pos = mul(mView, pos);
	//pos = mul(mProj, pos);
	psInput.Position = pos;
	return psInput;
}
/// <summary>
/// スキン有りシャドウマップ生成用の頂点シェーダー
/// </summary>
PSInput_ShadowMap VSMainSkin_ShadowMap(SVSIn In)
{
	PSInput_ShadowMap psInput = (PSInput_ShadowMap)0;
	float4x4 skinning = CalcSkinMatrix(In);
	float4 pos = mul(skinning, In.pos);
	pos = mul(mLightView, pos);
	pos = mul(mLightProj, pos);
	//pos = mul(mView, pos);
	//pos = mul(mProj, pos);
	psInput.Position = pos;
	return psInput;
}

/// <summary>
/// スキン無しカスケードシャドウマップ生成用の頂点シェーダー
/// </summary>
PSInput_ShadowMap VSMain_CascadeShadowMap(SVSIn In)
{
	PSInput_ShadowMap psInput = (PSInput_ShadowMap)0;
	float4 pos = mul(mWorld, In.pos);
	pos = mul(mLightViewProj[shadowMapNumber], pos);
	psInput.Position = pos;
	return psInput;
}

/// <summary>
/// スキン有りカスケードシャドウマップ生成用の頂点シェーダー
/// </summary>
PSInput_ShadowMap VSMainSkin_CascadeShadowMap(SVSIn In)
{
	PSInput_ShadowMap psInput = (PSInput_ShadowMap)0;
	float4x4 skinning = CalcSkinMatrix(In);
	float4 pos = mul(skinning, In.pos);
	pos = mul(mLightViewProj[shadowMapNumber], pos);
	psInput.Position = pos;
	return psInput;
}

/// <summary>
/// シャドウマップ生成用のピクセルシェーダー
/// </summary>
float4 PSMain_ShadowMap(PSInput_ShadowMap In) : SV_Target0
{
	//射影空間でのZ値を返す
	return In.Position.z / In.Position.w;
	//return 0.1f;
}