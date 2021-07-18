#pragma once
#include"DX12Setup.h"
#include"Window.h"
#include"DataType/FVertex.h"
#include "File Reader/ModelFileImporter.h"
#include<DirectXMath.h>
#include<vector>
#include<winnt.h>


#pragma comment (lib, "dxguid.lib")
class RednererFunction
{
	uint64_t m_FenceValues[Window::BufferCount] = {};
	// Vertex buffer for the cube.
	Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	// Index buffer for the cube.
	Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

	// Depth buffer, Depth Stencil View
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DSV;
	// Descriptor heap for depth buffer.
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;

	// Root signature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

	// Pipeline state object.
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;

	D3D12_VIEWPORT m_viewport; // area that output from rasterizer will be stretched to.

	D3D12_RECT m_ScissorRect; // the area to draw in. pixels outside that area will not be drawn onto

	float m_FoV;

	DirectX::XMMATRIX m_ModelMatrix;
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ProjectionMatrix;

	bool m_ContentLoaded;
	Window* m_wnd;

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
public:
	// Helper functions
	// Transition a resource
	void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
		Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

	// Clear a render target view.
	void ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor);

	// Clear the depth of a depth-stencil view.
	void ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f);

	// Create a GPU buffer.
	void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
		ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource,
		size_t numElements, size_t elementSize, const void* bufferData,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	// Resize the depth buffer to match the size of the client area.
	void ResizeDepthBuffer(int width, int height);

	//temp function for update , will change it after
	void OnUpdate(UpdateEventArgs& e);

	void CreateRootSign(ComPtr<ID3D12Device2> device, ComPtr<ID3D12RootSignature> rootSignature);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature()
	{
		return m_RootSignature;
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState()
	{
		return m_PipelineState;
	}
	void SetCommandList(ComPtr<ID3D12Device2> device, DX12Setup* setup);

	void CreateVertexBuffer(ComPtr<ID3D12Device2> device, size_t vertSize, std::vector<FVertex::Vertex>& vertices);

	void PopulateCommandList();
	 
	void CloseCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue);
	
	void LoadAssetData(FileImport::Import::MeshImport& mesh);

	void DrawTriangle();

};