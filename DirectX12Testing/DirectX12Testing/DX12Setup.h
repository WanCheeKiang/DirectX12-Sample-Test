#pragma once
// tutourial from https://www.3dgep.com/learning-directx-12-1/
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <algorithm>
#include <shellapi.h> // For CommandLineToArgvW

// The min/max macros conflict with like-named member functions.
// Only use std::min and std::max defined in <algorithm>.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

// In order to define a function called CreateWindow, the Windows macro needs to
// be undefined.
#if defined(CreateWindow)
#undef CreateWindow
#endif

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// D3D12 extension library.
#include "d3dx12.h"
#include"DX12Helpers.h"

#include<chrono>

class DX12Setup
{

	//DirectX 12 Setup
public:
	// The number of swap chain back buffers.
	static const uint8_t m_NumFrames = 3;
	// Use WARP adapter
	bool m_UseWarp = false;
	//window width and height
	uint32_t m_ClientWidth = 1280;
	uint32_t m_ClientHeight = 720;
private:
	// Set to true once the DX12 objects have been initialized.
	bool m_IsInitialized = false;

	// Window handle.
	HWND m_hWnd;
	// Window rectangle (used to toggle fullscreen state).
	RECT m_WindowRect;

	// DirectX 12 Objects
	ComPtr<ID3D12Device2> m_Device;
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<IDXGISwapChain4> m_SwapChain;
	ComPtr<ID3D12Resource> m_BackBuffers[m_NumFrames];
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocators[m_NumFrames];
	ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
	UINT m_RTVDescriptorSize;
	UINT m_CurrentBackBufferIndex;

	// Synchronization objects
	ComPtr<ID3D12Fence> m_Fence;
	uint64_t m_FenceValue = 0;
	uint64_t m_FrameFenceValues[m_NumFrames] = {};
	HANDLE m_FenceEvent;

	// By default, enable V-Sync.
	// Can be toggled with the V key.
	bool m_VSync = true;
	bool m_TearingSupported = false;
	// By default, use windowed mode.
	// Can be toggled with the Alt+Enter or F11
	bool m_Fullscreen = false;

public:
	static DX12Setup* GetSetup();
	HWND CreateWindow(const wchar_t* windowClassName, HINSTANCE hInst,
		const wchar_t* windowTitle, uint32_t width, uint32_t height);
	ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp);
	ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIAdapter4> adapter);
	ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
	bool CheckTearingSupport();
	ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd,
		ComPtr<ID3D12CommandQueue> commandQueue,
		uint32_t width, uint32_t height, uint32_t bufferCount);
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
	void UpdateRenderTargetViews(ComPtr<ID3D12Device2> device,
		ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap);
	ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE listType);
	ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12Device2> device,
		ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> device);
	HANDLE CreateEventHandle();
	uint64_t Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
		uint64_t& fenceValue);
	void WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
		std::chrono::milliseconds duration = std::chrono::milliseconds::max());
	void Flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
		uint64_t& fenceValue, HANDLE fenceEvent);

};
