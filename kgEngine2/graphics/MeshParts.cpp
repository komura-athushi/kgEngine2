#include "stdafx.h"
#include "MeshParts.h"
#include "Skeleton.h"
#include "Material.h"
#include "shadow/ShadowMap.h"

MeshParts::~MeshParts()
{
	for (auto& mesh : m_meshs) {
		//�C���f�b�N�X�o�b�t�@���폜�B
		for (auto& ib : mesh->m_indexBufferArray) {
			delete ib;
		}
		//�}�e���A�����폜�B
		for (auto& mat : mesh->m_materials) {
			delete mat;
		}
		//���b�V�����폜�B
		delete mesh;
	}
}
void MeshParts::InitFromTkmFile(
	const TkmFile& tkmFile, 
	const wchar_t* fxFilePath,
	const char* vsEntryPointFunc,
	const char* psEntryPointFunc,
	void* expandData,
	int expandDataSize
)
{
	m_meshs.resize(tkmFile.GetNumMesh());
	int meshNo = 0;
	tkmFile.QueryMeshParts([&](const TkmFile::SMesh& mesh) {
		//tkm�t�@�C���̃��b�V����񂩂烁�b�V�����쐬����B
		CreateMeshFromTkmMesh(mesh, meshNo, fxFilePath, vsEntryPointFunc, psEntryPointFunc);


		meshNo++;
	});
	//���ʒ萔�o�b�t�@�̍쐬�B
	for (int i = 0; i < CascadeShadowMap::SHADOWMAP_NUM + 1; i++) {
		m_commonConstantBuffer[i].Init(sizeof(SConstantBuffer), nullptr);

	}
	//���[�U�[�g���p�̒萔�o�b�t�@���쐬�B
	if (expandData) {
		m_expandConstantBuffer.Init(expandDataSize, nullptr);
		m_expandData = expandData;
	}
}

void MeshParts::CreateDescriptorHeaps()
{
	//�f�B�X�N���v�^�q�[�v�̓}�e���A���̐��������쐬�����B
	int numDescriptorHeap = 0;
	for (auto& mesh : m_meshs) {
		for (int matNo = 0; matNo < mesh->m_materials.size(); matNo++) {
			numDescriptorHeap++;
		}
	}
	for (int i = 0; i < CascadeShadowMap::SHADOWMAP_NUM + 1; i++) {
		//�f�B�X�N���v�^�q�[�v���h�J�b�Ɗm�ہB
		m_descriptorHeapList[i].resize(numDescriptorHeap);
		//�f�B�X�N���v�^�q�[�v���\�z���Ă����B
		int descriptorHeapNo = 0;
		for (auto& mesh : m_meshs) {
			for (int matNo = 0; matNo < mesh->m_materials.size(); matNo++) {
				DescriptorHeapList& descriptorHeapList = m_descriptorHeapList[i];
				DescriptorHeap& descriptorHeap = descriptorHeapList.at(descriptorHeapNo);
				//�f�B�X�N���v�^�q�[�v�Ƀf�B�X�N���v�^��o�^���Ă����B
				descriptorHeap.RegistShaderResource(0, mesh->m_materials[matNo]->GetAlbedoMap());		//�A���x�h�}�b�v�B
				descriptorHeap.RegistShaderResource(1, mesh->m_materials[matNo]->GetNormalMap());		//�@���}�b�v�B
				descriptorHeap.RegistShaderResource(2, mesh->m_materials[matNo]->GetSpecularMap());	//�X�y�L�����}�b�v�B
				descriptorHeap.RegistShaderResource(3, m_boneMatricesStructureBuffer);	//�{�[��
				descriptorHeap.RegistShaderResource(4, g_graphicsEngine->GetShadowMap()->GetShadowMapTexture());  //�V���h�E�}�b�v
				for (int i = 0; i < CascadeShadowMap::SHADOWMAP_NUM; i++) {
					descriptorHeap.RegistShaderResource(4 + i + 1, g_graphicsEngine->GetCascadeShadowMap()->GetShadowMapTexture(i));
				}
			
				descriptorHeap.RegistConstantBuffer(0, m_commonConstantBuffer[i]);
				if (m_expandConstantBuffer.IsValid()) {
					descriptorHeap.RegistConstantBuffer(1, m_expandConstantBuffer);
				}
				//�f�B�X�N���v�^�q�[�v�ւ̓o�^���m�肳����B
				descriptorHeap.Commit();
				descriptorHeapNo++;
			}
		}
	}
	m_isInitDescriptorHeap = true;
}
void MeshParts::CreateMeshFromTkmMesh(
	const TkmFile::SMesh& tkmMesh, 
	int meshNo,
	const wchar_t* fxFilePath,
	const char* vsEntryPointFunc,
	const char* psEntryPointFunc)
{
	//���_�o�b�t�@���쐬�B
	int numVertex = (int)tkmMesh.vertexBuffer.size();
	int vertexStride = sizeof(TkmFile::SVertex);
	auto mesh = new SMesh;
	mesh->skinFlags.reserve(tkmMesh.materials.size());
	mesh->m_vertexBuffer.Init(vertexStride * numVertex, vertexStride);
	mesh->m_vertexBuffer.Copy((void*)&tkmMesh.vertexBuffer[0]);

	auto SetSkinFlag = [&](int index) {
		if (tkmMesh.vertexBuffer[index].skinWeights.x > 0.0f) {
			//�X�L��������B
			mesh->skinFlags.push_back(1);
		}
		else {
			//�X�L���Ȃ��B
			mesh->skinFlags.push_back(0);
		}
	};
	//�C���f�b�N�X�o�b�t�@���쐬�B
	if (!tkmMesh.indexBuffer16Array.empty()) {
		//�C���f�b�N�X�̃T�C�Y��2byte
		mesh->m_indexBufferArray.reserve(tkmMesh.indexBuffer16Array.size());
		for (auto& tkIb : tkmMesh.indexBuffer16Array) {
			auto ib = new IndexBuffer;
			ib->Init(static_cast<int>(tkIb.indices.size()) * 2, 2);
			ib->Copy((void*)&tkIb.indices.at(0));

			//�X�L�������邩�ǂ�����ݒ肷��B
			SetSkinFlag(tkIb.indices[0]);

			mesh->m_indexBufferArray.push_back(ib);
		}
	}
	else {
		//�C���f�b�N�X�̃T�C�Y��4byte
		mesh->m_indexBufferArray.reserve(tkmMesh.indexBuffer32Array.size());
		for (auto& tkIb : tkmMesh.indexBuffer32Array) {
			auto ib = new IndexBuffer;
			ib->Init(static_cast<int>(tkIb.indices.size()) * 4, 4);
			ib->Copy((void*)&tkIb.indices.at(0));

			//�X�L�������邩�ǂ�����ݒ肷��B
			SetSkinFlag(tkIb.indices[0]);

			mesh->m_indexBufferArray.push_back(ib);
		}
	}
	//�}�e���A�����쐬�B
	mesh->m_materials.reserve(tkmMesh.materials.size());
	for (auto& tkmMat : tkmMesh.materials) {
		auto mat = new Material;
		mat->InitFromTkmMaterila(tkmMat, fxFilePath, vsEntryPointFunc, psEntryPointFunc);
		mesh->m_materials.push_back(mat);
	}

	m_meshs[meshNo] = mesh;
	
}

void MeshParts::BindSkeleton(Skeleton& skeleton)
{
	m_skeleton = &skeleton;
	//�\�����o�b�t�@���쐬����B
	m_boneMatricesStructureBuffer.Init(
		sizeof(Matrix),
		m_skeleton->GetNumBones(),
		m_skeleton->GetBoneMatricesTopAddress()
	);
}
void MeshParts::Draw(
	RenderContext& rc,
	const Matrix& mWorld,
	const Matrix& mView,
	const Matrix& mProj,
	EnRenderMode renderMode,
	int shadowMapNumber
)
{
#if 1
	if (!m_isInitDescriptorHeap) {
		//�f�B�X�N���v�^�q�[�v���쐬�B
		CreateDescriptorHeaps();
	}

	//���b�V�����ƂɃh���[
	//�v���~�e�B�u�̃g�|���W�[�̓g���C�A���O�����X�g�̂݁B
	rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto shadowMap = g_graphicsEngine->GetShadowMap();
	auto cascadeMap = g_graphicsEngine->GetCascadeShadowMap();
	//�萔�o�b�t�@���X�V����B
	SConstantBuffer cb;
	cb.mWorld = mWorld;
	cb.mLightViewProj[0] = cascadeMap->GetLightViewProjMatrix(0);
	cb.mLightViewProj[1] = cascadeMap->GetLightViewProjMatrix(1);
	cb.mLightViewProj[2] = cascadeMap->GetLightViewProjMatrix(2);
	if (renderMode == enRenderMode_CreateCascadeShadowMap) {
		cb.shadowMapNumber = cascadeMap->GetShadowMapNumber();
	}
	else if (renderMode == enRenderMode_CreateShadowMap) {
		cb.mView = shadowMap->GetLightViewMatrix();
		cb.mProj = shadowMap->GetLightProjMatrix();
		//cb.mView = mView;
		//cb.mProj = mProj;
	}
	else if(renderMode == enRenderMode_Normal){
		cb.mView = mView;
		cb.mProj = mProj;
		//cb.mView = shadowMap->GetLightViewMatrix();
		//cb.mProj = shadowMap->GetLightProjMatrix();
	}
	cb.mLightView = shadowMap->GetLightViewMatrix();
	cb.mLightProj = shadowMap->GetLightProjMatrix();
	//cb.mLightView = mView;
	//cb.mProj = mProj;

	if (m_isShadowReciever) {
		cb.isShadowReciever = 1;
	}
	else {
		cb.isShadowReciever = 0;
	}
	Matrix m = g_camera3D->GetViewMatrix() * g_camera3D->GetProjectionMatrix();


	int descriptorHeapNumber = 0;
	if (renderMode == enRenderMode_CreateCascadeShadowMap) {
		descriptorHeapNumber = cascadeMap->GetShadowMapNumber() + 1;
	}

	m_commonConstantBuffer[descriptorHeapNumber].CopyToVRAM(&cb);

	if (m_expandData) {
		m_expandConstantBuffer.CopyToVRAM(m_expandData);
	}
	if (m_boneMatricesStructureBuffer.IsInited()) {
		//�{�[���s����X�V����B
		m_boneMatricesStructureBuffer.Update(m_skeleton->GetBoneMatricesTopAddress());
	}

	

	int descriptorHeapNo = 0;
	for (auto& mesh : m_meshs) {
		//���_�o�b�t�@��ݒ�B
		rc.SetVertexBuffer(mesh->m_vertexBuffer);
		//�}�e���A�����ƂɃh���[�B
		for (int matNo = 0; matNo < mesh->m_materials.size(); matNo++) {
			//���̃}�e���A�����\���Ă��郁�b�V���̕`��J�n�B
			mesh->m_materials[matNo]->BeginRender(rc, mesh->skinFlags[matNo], renderMode);
			//�f�B�X�N���v�^�q�[�v��o�^�B
			DescriptorHeapList& descriptorHeapList = m_descriptorHeapList[descriptorHeapNumber];
			DescriptorHeap& descriptorHeap = descriptorHeapList.at(descriptorHeapNo);
			rc.SetDescriptorHeap(descriptorHeap);
			//�C���f�b�N�X�o�b�t�@��ݒ�B
			auto& ib = mesh->m_indexBufferArray[matNo];
			rc.SetIndexBuffer(*ib);

			//�h���[�B
			rc.DrawIndexed(ib->GetCount());
			descriptorHeapNo++;
		}
	}
#endif
}