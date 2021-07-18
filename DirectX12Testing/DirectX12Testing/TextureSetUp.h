#pragma once
#include"DX12Setup.h"
#include"Window.h"
#include "DataType/TempResource.h"

#include<DirectXMath.h>
#include<vector>
#include<winnt.h>
#pragma comment (lib, "dxguid.lib")

// TODO : change create type function output type from void to HRSULT for checking failed 
class TextureSetUp
{
// Note: ComPtr's are CPU objects but this resource needs to stay in scope until
 // the command list that references it has finished executing on the GPU.
 // We will flush the GPU at the end of this method to ensure the resource is not
 // prematurely destroyed.
	ComPtr<ID3D12Resource> m_texture;
	ComPtr<ID3D12Resource> textureUploadHeap;
	ComPtr<ID3D12Resource> m_constantBuffer;
	//ComPtr<ID3D12DescriptorHeap> m_rtvDescHeap; // TODO :delete descHeap variable OR create a set function for local var? rtvdescheap is in DX12Setup.h need to put descheap car to the same place or put a get function  
	ComPtr<ID3D12DescriptorHeap> m_srvDescHeap;
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	ConstantBuffer m_constantBufferData;
	UINT8* m_pCbvDataBegin;
	
	//create dsv TODO
	//rtv > dsv > cbv >
	

public:
	void Create2dTexture(UINT textureWidth, UINT textureHeight, ComPtr<ID3D12Resource>& texture, ComPtr<ID3D12Device2> device);
	ComPtr<ID3D12Resource> GetTexture();
	void CreateGPUUploadBuffer(ComPtr<ID3D12Resource>& texture, ComPtr<ID3D12Device2> device);
	void CreateRTVDescriptorHeap(ComPtr<ID3D12Device2> device, ComPtr<ID3D12Resource> rtv ,float frameCount, UINT m_rtvDescriptorSize);
	void CreateSRV(ComPtr<ID3D12Device2> device, D3D12_RESOURCE_DESC* textureDesc);
	void CretaeSRVDescriptorHeap(ComPtr<ID3D12Device2> device, float frameCount, UINT m_rtvDescriptorSize);
	void CreateCBV(ComPtr<ID3D12Device2> device);
	void UpdateCBV();
	void LoadTextureData(ComPtr<ID3D12Device2> device, ComPtr<ID3D12GraphicsCommandList>& commandList, const wchar_t* szFile, const UINT& cbvIndex, ComPtr<ID3D12Resource>& buffer, ComPtr<ID3D12Resource>& upload, UINT32 cbvDescriptorSize);
	
};