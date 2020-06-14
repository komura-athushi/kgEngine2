#pragma once

inline void RenderContext::SetDescriptorHeap(DescriptorHeap& descHeap)
{
	m_descriptorHeaps[0] = descHeap.Get();
	m_commandList->SetDescriptorHeaps(1, m_descriptorHeaps);

	//ディスクリプタテーブルに登録する。
	if (descHeap.IsRegistConstantBuffer()) {
		SetGraphicsRootDescriptorTable(0, descHeap.GetConstantBufferGpuDescritorStartHandle());
	}
	if (descHeap.IsRegistShaderResource()) {
		SetGraphicsRootDescriptorTable(1, descHeap.GetShaderResourceGpuDescritorStartHandle());
	}
	if (descHeap.IsRegistUavResource()) {
		SetGraphicsRootDescriptorTable(2, descHeap.GetUavResourceGpuDescritorStartHandle());
	}
}
inline void RenderContext::SetComputeDescriptorHeap(DescriptorHeap& descHeap)
{
	m_descriptorHeaps[0] = descHeap.Get();
	m_commandList->SetDescriptorHeaps(1, m_descriptorHeaps);

	//ディスクリプタテーブルに登録する。
	if (descHeap.IsRegistConstantBuffer()) {
		SetComputeRootDescriptorTable(0, descHeap.GetConstantBufferGpuDescritorStartHandle());
	}
	if (descHeap.IsRegistShaderResource()) {
		SetComputeRootDescriptorTable(1, descHeap.GetShaderResourceGpuDescritorStartHandle());
	}
	if (descHeap.IsRegistUavResource()) {
		SetComputeRootDescriptorTable(2, descHeap.GetUavResourceGpuDescritorStartHandle());
	}
}