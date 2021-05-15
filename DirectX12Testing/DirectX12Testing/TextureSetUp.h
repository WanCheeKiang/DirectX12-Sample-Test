#pragma once
#include"DX12Setup.h"
#include"Window.h"
#include<DirectXMath.h>
#include<vector>
#include<winnt.h>
#pragma comment (lib, "dxguid.lib")

class TextureSetUp
{
// Note: ComPtr's are CPU objects but this resource needs to stay in scope until
 // the command list that references it has finished executing on the GPU.
 // We will flush the GPU at the end of this method to ensure the resource is not
 // prematurely destroyed.
	ComPtr<ID3D12Resource> m_texture;
	ComPtr<ID3D12Resource> textureUploadHeap;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;

public:
	void Create2dTexture(UINT textureWidth, UINT textureHeight, ComPtr<ID3D12Resource>& texture, ID3D12Device2* device);
	ComPtr<ID3D12Resource> GetTexture();
	void CreateGPUUploadBuffer(ComPtr<ID3D12Resource>& texture, ID3D12Device2* device);
	void CreateDescriptorHeaps(ID3D12Device2* device, float frameCount, UINT m_rtvDescriptorSize);
	void CreateSRV(ID3D12Device2* device, D3D12_RESOURCE_DESC* textureDesc);
};