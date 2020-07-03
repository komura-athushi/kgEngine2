

static const int NUM_CASCADES = 3;


//���f���p�̒萔�o�b�t�@
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
/// �V���h�E�}�b�v�p�̒萔�o�b�t�@�B
/// </summary>
cbuffer ShadowMapCb : register(b1) {
	float4x4 lightViewProjMatrix;	//���C�g�r���[�v���W�F�N�V�����s��B
}

//���_�V�F�[�_�[�ւ̓��́B
struct SVSIn {
	float4 pos 		: POSITION;		//���f���̒��_���W�B
	float3 normal	: NORMAL;		//�@���B
	float2 uv 		: TEXCOORD0;	//UV���W�B
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

//�s�N�Z���V�F�[�_�[�ւ̓��́B
struct SPSIn {
	float4 pos 			: SV_POSITION;	//�X�N���[����Ԃł̃s�N�Z���̍��W�B
	float3 normal		: NORMAL;		//�@���B
	float2 uv 			: TEXCOORD0;	//uv���W�B
	float3 worldPos		: TEXCOORD1;	//���[���h��Ԃł̃s�N�Z���̍��W�B
	float4 posInLVP[NUM_CASCADES]		: TEXCOORD2;	//���C�g�r���[�v���W�F�N�V������Ԃł̍��W
};

/// <summary>
/// �V���h�E�}�b�v�p�̃s�N�Z���V�F�[�_�ւ̓��͍\���́B
/// </summary>
struct PSInput_ShadowMap {
	float4 Position 			: SV_POSITION;	//���W�B
};

//�s�N�Z���V�F�[�_�[����̏o��
struct SPSOut {
	float4 albedo : SV_Target0;		//�A���x�h
	float4 normal : SV_Target1;		//�@��
	float shadow : SV_Target2;		//�V���h�E
};

//���f���e�N�X�`���B
Texture2D<float4> g_texture : register(t0);
//�@���}�b�v
Texture2D<float4> g_normal : register(t1);
//�X�y�L�����}�b�v
Texture2D<float4> g_specularMap : register(t2);
//�{�[���s��
StructuredBuffer<float4x4> boneMatrix : register(t3);
//�V���h�E�}�b�v
Texture2D<float4> g_shadowMap : register(t4);
//�J�X�P�[�h�V���h�E�}�b�v
Texture2D<float4> g_cascadeShadowMap1 : register(t5);
Texture2D<float4> g_cascadeShadowMap2 : register(t6);
Texture2D<float4> g_cascadeShadowMap3 : register(t7);

//�T���v���X�e�[�g�B
sampler g_sampler : register(s0);

/*!
 *@brief	�X�L���s����v�Z�B
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
/// ���f���p�̒��_�V�F�[�_�[�̃G���g���[�|�C���g�B
/// </summary>
SPSIn VSMain(SVSIn vsIn)
{
	SPSIn psIn;
	
	float4 worldPos = mul(mWorld, vsIn.pos);			//���f���̒��_�����[���h���W�n�ɕϊ��B
	psIn.pos = worldPos;
	psIn.worldPos = psIn.pos.xyz;
	if (isShadowReciever == 1) {
		//���C�g�r���[�v���W�F�N�V�������W�ɕϊ�
		for (int i = 0; i < NUM_CASCADES; i++) {
			psIn.posInLVP[i] = mul(mLightViewProj[i], worldPos);
		}
	}
	psIn.pos = mul(mView, psIn.pos);			//���[���h���W�n����J�������W�n�ɕϊ��B
	psIn.pos = mul(mProj, psIn.pos);			//�J�������W�n����X�N���[�����W�n�ɕϊ��B
	psIn.normal = mul(mWorld, vsIn.normal);		//�@�������[���h���W�n�ɕϊ��B
	psIn.uv = vsIn.uv;
	

	return psIn;
}

/// <summary>
/// �X�L�����胂�f���p�̒��_�V�F�[�_�[�̃G���g���[�|�C���g�B
/// </summary>
SPSIn VSMainSkin(SVSIn vsIn)
{
	SPSIn psIn;

	float4x4 skinning = CalcSkinMatrix(vsIn);
	float4 worldPos = mul(skinning, vsIn.pos);
	psIn.pos = worldPos;
	psIn.worldPos = psIn.pos.xyz;
	if (isShadowReciever == 1) {
		//���C�g�r���[�v���W�F�N�V�������W�ɕϊ�
		for (int i = 0; i < NUM_CASCADES; i++) {
			psIn.posInLVP[i] = mul(mLightViewProj[i], worldPos);
		}
	}
	psIn.pos = mul(mView, psIn.pos);			//���[���h���W�n����J�������W�n�ɕϊ��B
	psIn.pos = mul(mProj, psIn.pos);			//�J�������W�n����X�N���[�����W�n�ɕϊ��B
	psIn.normal = mul(mWorld, vsIn.normal);		//�@�������[���h���W�n�ɕϊ��B
	psIn.uv = vsIn.uv;

	return psIn;
}

/// <summary>
/// ���f���p�̃s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g
/// </summary>
SPSOut PSMain(SPSIn psIn) : SV_Target0
{
	float4 color = g_texture.Sample(g_sampler,psIn.uv);

	////�g�U���ˌ�////
	/*float3 lig = float3(0.0f, 0.0f, 0.0f);
	lig = max(0.0f, dot(psIn.normal * -1.0f, directionR)) * colorR;
	lig += max(0.0f, dot(psIn.normal * -1.0f, directionG)) * colorG;
	lig += max(0.0f, dot(psIn.normal * -1.0f, directionB)) * colorB;

	////���ʔ���////
	//1 ���˃x�N�g��R�����߂�B
	float3 R = 0.f;
	R += float3(0.0f, 0.0f, -1.0f) + -2 * dot(psIn.normal, -1 * float3(0.0f, 0.0f, -1.0f)) * psIn.normal;

	//2 ���_���烉�C�g�𓖂Ă镨�̂ɐL�т�VectorE�����߂�
	float3 E = normalize(psIn.worldPos - psIn.eyePos);

	//1��2�ŋ��܂���Vector�̂Ȃ��Ȃ��v�Z����B
	//�X�y�L�������˂̋��������߂�B
	float specPower = max(0, dot(R, -E));

	//3 �X�y�L�������˂����C�g�ɉ��Z����B
	lig += colorR.xyz * pow(specPower, psIn.specPow);
	lig += colorG.xyz * pow(specPower, psIn.specPow);
	lig += colorB.xyz * pow(specPower, psIn.specPow);*/

	////�ŏI�J���[�̌���////
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//finalColor.xyz = color.xyz * lig;
	finalColor.xyz = color.xyz;
	

	SPSOut spsOut;
	spsOut.albedo = finalColor;
	spsOut.normal.xyz = psIn.normal;
	spsOut.normal.w = 1.0f;
	
	spsOut.shadow = 1.0f;
	//spsOut.shadow = color.x;
		//�V���h�E���V�[�o�[�B
	if (isShadowReciever == 1) {
		for (int i = 0; i < NUM_CASCADES; i++) {
			//LVP��Ԃ��猩�����̍ł���O�̐[�x�l���V���h�E�}�b�v����擾����B
		//�v���W�F�N�V�����s����V���h�E�}�b�v��UV���W�ɕϊ����Ă���
			float2 shadowMapUV = psIn.posInLVP[i].xy / psIn.posInLVP[i].w;
			shadowMapUV *= float2(0.5f, -0.5f);
			shadowMapUV += 0.5f;
			//spsOut.shadow = shadowMapUV.x;
			//�V���h�E�}�b�v��UV���W�͈͓����ǂ����𔻒肷��B
			if (shadowMapUV.x < 1.0f
				&& shadowMapUV.x > 0.0f
				&& shadowMapUV.y < 1.0f
				&& shadowMapUV.y > 0.0f
				) {
				///LVP��Ԃł̐[�x�l���v�Z�B
				float zInLVP = psIn.posInLVP[i].z / psIn.posInLVP[i].w;
				float zInShadowMap;
				//�V���h�E�}�b�v�ɏ������܂�Ă���[�x�l���擾�B
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
					//�e�������Ă���̂ŁA�����キ����
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
/// �X�L�������V���h�E�}�b�v�����p�̒��_�V�F�[�_�[
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
/// �X�L���L��V���h�E�}�b�v�����p�̒��_�V�F�[�_�[
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
/// �X�L�������J�X�P�[�h�V���h�E�}�b�v�����p�̒��_�V�F�[�_�[
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
/// �X�L���L��J�X�P�[�h�V���h�E�}�b�v�����p�̒��_�V�F�[�_�[
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
/// �V���h�E�}�b�v�����p�̃s�N�Z���V�F�[�_�[
/// </summary>
float4 PSMain_ShadowMap(PSInput_ShadowMap In) : SV_Target0
{
	//�ˉe��Ԃł�Z�l��Ԃ�
	return In.Position.z / In.Position.w;
	//return 0.1f;
}