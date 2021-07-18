#include "RendererFunction.h"

#include "Application.h"
#include "CommandQueue.h"
#include "DX12Helpers.h"
#include "Window.h"
#include "TextureSetUp.h"
#include "DataType/TempResource.h"
#include "TextureSetUp.h"


#include <wrl.h>
#include <combaseapi.h>

using namespace Microsoft::WRL;

#include <d3dcompiler.h>

#include <algorithm> // For std::min and std::max.

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

using namespace DirectX;
using namespace std;
using namespace FVertex;
using namespace FileImport;
using namespace Import;
// temp data



void RednererFunction::TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, Microsoft::WRL::ComPtr<ID3D12Resource> resource,
	D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), beforeState, afterState);

	commandList->ResourceBarrier(1, &barrier);
}
void RednererFunction::ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor)
{
	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void RednererFunction::ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth)
{
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void RednererFunction::UpdateBufferResource(
	ComPtr<ID3D12GraphicsCommandList2> commandList,
	ID3D12Resource** pDestinationResource,
	ID3D12Resource** pIntermediateResource,
	size_t numElements, size_t elementSize, const void* bufferData,
	D3D12_RESOURCE_FLAGS flags)
{
	ComPtr<ID3D12Device2> device = Application::Get().GetDevice();

	size_t bufferSize = numElements * elementSize;
	// Create a committed resource for the GPU resource in a default heap.
	CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
	ThrowIfFailed(device->CreateCommittedResource(
		&HeapProperties::Default,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(pDestinationResource)));
	// Create an committed resource for the upload.
	if (bufferData)
	{
		
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize); //re-using var 
		ThrowIfFailed(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pIntermediateResource)));
		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = bufferData;
		subresourceData.RowPitch = bufferSize;
		subresourceData.SlicePitch = subresourceData.RowPitch;

		UpdateSubresources(commandList.Get(),
			*pDestinationResource, *pIntermediateResource,
			0, 0, 1, &subresourceData);
	}
}

void RednererFunction::ResizeDepthBuffer(int width, int height)
{
	if (m_ContentLoaded)
	{
		// Flush any GPU commands that might be referencing the depth buffer.
		Application::Get().Flush();

		width = std::max(1, width);
		height = std::max(1, height);

		auto device = Application::Get().GetDevice();
		// Resize screen dependent resources.
		// Create a depth buffer.
		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC texResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		ThrowIfFailed(device->CreateCommittedResource(
			&HeapProperties::Default,
			D3D12_HEAP_FLAG_NONE,
			&texResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&m_DSV)
			));      
		// Update the depth-stencil view.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		device->CreateDepthStencilView(m_DSV.Get(), &dsv,
			m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
	}
}

void RednererFunction::OnUpdate(UpdateEventArgs& e)
{
	
}

void RednererFunction::CreateRootSign(ComPtr<ID3D12Device2> device, ComPtr<ID3D12RootSignature> rootSignature)
{
	// Create the root signature.
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_ROOT_PARAMETER1 rootParameters[1];
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

	HRESULT hr;
	//dx11 blob > root signature in dx12, it is similar but is not the same  
// create root signature
//	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
//	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

//	ID3DBlob* signature;
//
//	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//
//	hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//
//	return hr;
}

void RednererFunction::SetCommandList(ComPtr<ID3D12Device2> device, DX12Setup* setup)
{
	setup = DX12Setup::GetSetup();
	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, setup->m_CommandAllocators->Get(), m_PipelineState.Get(), IID_PPV_ARGS(&setup->m_CommandList)));

}

void RednererFunction::CreateVertexBuffer(ComPtr<ID3D12Device2> device, size_t vertSize, vector<FVertex::Vertex>& vertices)
{
	const UINT vertexBufferSize = sizeof(vertices.data()); // 0 is temp //sizeof();

	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
	ThrowIfFailed(device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_VertexBuffer)));

	// Copy the data to the vertex buffer.
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
	ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, vertices.data(), sizeof(vertices.data()));
	m_VertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_vertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
	m_vertexBufferView.SizeInBytes = vertexBufferSize;


}

void RednererFunction::PopulateCommandList()
{
	DX12Setup* dx12setup = DX12Setup::GetSetup();

	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(dx12setup->m_CommandAllocators->Reset());
	
	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ComPtr<ID3D12GraphicsCommandList> CommandList = dx12setup->m_CommandList;
	ThrowIfFailed(CommandList->Reset(dx12setup->m_CommandAllocators->Get(), m_PipelineState.Get()));
	
	// Set necessary state.
	CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	CommandList->RSSetViewports(1, &m_viewport);
	CommandList->RSSetScissorRects(1, &m_ScissorRect);
	
	// Indicate that the back buffer will be used as a render target.
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12setup->m_BackBuffers[dx12setup->m_CurrentBackBufferIndex].Get(),
																					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CommandList->ResourceBarrier(
		1, 
		&resourceBarrier);
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(dx12setup->m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), dx12setup->m_CurrentBackBufferIndex, dx12setup->m_RTVDescriptorSize);
	CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	
	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	CommandList->DrawInstanced(3, 1, 0, 0);
	
	// Indicate that the back buffer will now be used to present.
	ZeroMemory(&resourceBarrier,sizeof(resourceBarrier));
	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12setup->m_BackBuffers[dx12setup->m_CurrentBackBufferIndex].Get(),
															D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CommandList->ResourceBarrier(
		1, 
		&resourceBarrier);
	
	ThrowIfFailed(CommandList->Close());

}

void RednererFunction::CloseCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue)
{
	  // Close the command list and execute it to begin the initial GPU setup.
    ThrowIfFailed(commandList->Close());
    ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void RednererFunction::LoadAssetData(FileImport::Import::MeshImport& mesh)
{
	DX12Setup* dx12setup = DX12Setup::GetSetup();
	HRESULT hr;
	TextureSetUp texsetup;
	CreateVertexBuffer(dx12setup->GetDevice(), sizeof(mesh.vertices), mesh.vertices);
	for (unsigned int i = 0; i <= mesh.textures.size(); i++)
	{
		//hr = texsetup.LoadTextureData(dx12setup->GetDevice(), dx12setup->m_CommandList,mesh.textures[i], )
	}
}









