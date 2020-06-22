
cbuffer cb : register(b0){
	float4x4 mvp;	
	float4 mulColor;
};

cbuffer DirectionLight : register(b1){
	float3 ligColor;		//ライトのカラー
	float3 ligDirection;	//ライトの方向。
};
struct VSInput{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

Texture2D<float4> Texture : register(t0);	 //カラーテクスチャ
sampler Sampler : register(s0);

PSInput VSMain(VSInput In) 
{
	PSInput psIn;
	psIn.pos = mul( mvp, In.pos );
	psIn.uv = In.uv;
	return psIn;
}

float4 PSMain(PSInput In) : SV_Target0
{
	float4 Color = Texture.Sample(Sampler,In.uv);
	return Color;

}