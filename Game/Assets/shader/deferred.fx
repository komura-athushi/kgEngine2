
cbuffer cb : register(b0){
	float4x4 mvp;	
	float4 mulColor;
};

cbuffer DirectionLight : register(b1){
	float3 ligDirection;	//���C�g�̕����B
	float3 ligColor;		//���C�g�̃J���[

};
struct VSInput{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

Texture2D<float4> albedoTexture : register(t0);	//�A���x�h�B
Texture2D<float4> normalTexture : register(t1);	//�@���B
sampler Sampler : register(s0);

PSInput VSMain(VSInput In) 
{
	PSInput psIn;
	psIn.pos = mul( mvp, In.pos );
	psIn.uv = In.uv;
	return psIn;
}
float4 PSMain( PSInput In ) : SV_Target0
{
	//step-7 G-Buffer�̓��e���g���ă��C�e�B���O�B
	float4 albedo = albedoTexture.Sample(Sampler, In.uv);
	float3 normal = normalTexture.Sample(Sampler, In.uv).xyz;
	normal = (normal * 2.0f)-1.0f;
	//���C�g���v�Z�B
	float3 lig = 0.0f;
	float t = max( 0.0f, dot( normal, ligDirection) * -1.0f);
	lig = ligColor * t;
	float4 finalColor = albedo;
	finalColor.xyz *= lig;
	//finalColor.xyz = ligColor;
	return finalColor;
}