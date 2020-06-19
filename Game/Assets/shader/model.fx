

//���f���p�̒萔�o�b�t�@
cbuffer ModelCb : register(b0) {
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
	float3	eyesssPos;
	float	specPow;
};


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

//�{�[���s��
StructuredBuffer<float4x4> boneMatrix : register(t3);
//StructuredBuffer boneMatrix : register(t1);

//�s�N�Z���V�F�[�_�[�ւ̓��́B
struct SPSIn {
	float4 pos 			: SV_POSITION;	//�X�N���[����Ԃł̃s�N�Z���̍��W�B
	float3 normal		: NORMAL;		//�@���B
	float2 uv 			: TEXCOORD0;	//uv���W�B
	float3 worldPos		: TEXCOORD1;	//���[���h��Ԃł̃s�N�Z���̍��W�B
	float3 eyePos		: TEXCOORD2;
	float  specPow : TEXCOORD3;
};

//�s�N�Z���V�F�[�_�[����̏o��
struct SPSOut {
	float4 albedo : SV_Target0;		//�A���x�h
	float3 normal : SV_Target1;		//�@��
};

//���f���e�N�X�`���B
Texture2D<float4> g_texture : register(t0);
//�@���}�b�v
Texture2D<float4> g_normal : register(t1);
//�X�y�L�����}�b�v
Texture2D<float4> g_specular : register(t2);

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
SPSIn VSMain(SVSIn vsIn, uniform bool hasSkin)
{
	SPSIn psIn;

	psIn.pos = mul(mWorld, vsIn.pos);			//���f���̒��_�����[���h���W�n�ɕϊ��B
	psIn.worldPos = psIn.pos.xyz;
	psIn.pos = mul(mView, psIn.pos);			//���[���h���W�n����J�������W�n�ɕϊ��B
	psIn.pos = mul(mProj, psIn.pos);			//�J�������W�n����X�N���[�����W�n�ɕϊ��B
	psIn.normal = mul(mWorld, vsIn.normal);		//�@�������[���h���W�n�ɕϊ��B
	psIn.uv = vsIn.uv;
	//psIn.eyePos = vsIn.eyesssPos;
	//psIn.specPow = vsIn.specPow;

	return psIn;
}

/// <summary>
/// �X�L�����胂�f���p�̒��_�V�F�[�_�[�̃G���g���[�|�C���g�B
/// </summary>
SPSIn VSMainSkin(SVSIn vsIn)
{
	SPSIn psIn;

	float4x4 skinning = CalcSkinMatrix(vsIn);
	psIn.pos = mul(skinning, vsIn.pos);
	psIn.worldPos = psIn.pos.xyz;
	psIn.pos = mul(mView, psIn.pos);			//���[���h���W�n����J�������W�n�ɕϊ��B
	psIn.pos = mul(mProj, psIn.pos);			//�J�������W�n����X�N���[�����W�n�ɕϊ��B
	psIn.normal = mul(mWorld, vsIn.normal);		//�@�������[���h���W�n�ɕϊ��B
	psIn.uv = vsIn.uv;
	//psIn.eyePos = vsIn.eyesssPos;
	//psIn.specPow = vsIn.specPow;

	return psIn;
}

/// <summary>
/// ���f���p�̃s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g
/// </summary>
SPSOut PSMain(SPSIn psIn)
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
	spsOut.normal = psIn.normal;

	return spsOut;
	//return float4(specPow, 0.0f, 0.0f, 1.0f);    
}
