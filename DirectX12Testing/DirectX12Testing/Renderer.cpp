#include "Renderer.h"
#include "RendererFunction.h"
DX12Renderer* DX12Renderer::GetRenderer()
{
	static DX12Renderer renderer;
	return &renderer;
}

void DX12Renderer::Update()
{
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	elapsedSeconds += deltaTime.count() * 1e-9;
	if (elapsedSeconds > 1.0)
	{
		char buffer[500];
		auto fps = frameCounter / elapsedSeconds;
		sprintf_s(buffer, 500, "FPS: %f\n", fps); // causing corrupted string 
		OutputDebugStringA(/*(LPCWSTR)*/buffer);

		frameCounter = 0;
		elapsedSeconds = 0.0;
	}
}

void DX12Renderer::Render()
{
	//m_dx12Setup* m_dx12Setup = m_dx12Setup::GetSetup();
	auto commandAllocator = m_dx12Setup->m_CommandAllocators[m_dx12Setup->m_CurrentBackBufferIndex];
	auto backBuffer = m_dx12Setup->m_BackBuffers[m_dx12Setup->m_CurrentBackBufferIndex];

	commandAllocator->Reset();
	m_dx12Setup->m_CommandList->Reset(commandAllocator.Get(), nullptr);

	// Clear the render target.
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		m_dx12Setup->m_CommandList->ResourceBarrier(1, &barrier);
		FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_dx12Setup->m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_dx12Setup->m_CurrentBackBufferIndex, m_dx12Setup->m_RTVDescriptorSize);

		m_dx12Setup->m_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	}

	// Present
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_dx12Setup->m_CommandList->ResourceBarrier(1, &barrier);
		ThrowIfFailed(m_dx12Setup->m_CommandList->Close());

		ID3D12CommandList* const commandLists[] = {
			m_dx12Setup->m_CommandList.Get()
		};
		m_dx12Setup->m_CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
		UINT syncInterval = m_dx12Setup->m_VSync ? 1 : 0;
		UINT presentFlags = m_dx12Setup->m_TearingSupported && !m_dx12Setup->m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		ThrowIfFailed(m_dx12Setup->m_SwapChain->Present(syncInterval, presentFlags));

		m_dx12Setup->m_FrameFenceValues[m_dx12Setup->m_CurrentBackBufferIndex] = m_dx12Setup->Signal(m_dx12Setup->m_CommandQueue, m_dx12Setup->m_Fence, m_dx12Setup->m_FenceValue);
		m_dx12Setup->m_CurrentBackBufferIndex = m_dx12Setup->m_SwapChain->GetCurrentBackBufferIndex();

		m_dx12Setup->WaitForFenceValue(m_dx12Setup->m_Fence, m_dx12Setup->m_FrameFenceValues[m_dx12Setup->m_CurrentBackBufferIndex], m_dx12Setup->m_FenceEvent);
	}



}

void DX12Renderer::Resize(uint32_t width, uint32_t height)
{
	
	if (m_dx12Setup->m_ClientWidth != width || m_dx12Setup->m_ClientHeight != height)
	{
		// Don't allow 0 size swap chain back buffers.
		m_dx12Setup->m_ClientWidth = std::max(1u, width);
		m_dx12Setup->m_ClientHeight = std::max(1u, height);

		// Flush the GPU queue to make sure the swap chain's back buffers
		// are not being referenced by an in-flight command list.
		m_dx12Setup->Flush(m_dx12Setup->m_CommandQueue, m_dx12Setup->m_Fence, m_dx12Setup->m_FenceValue, m_dx12Setup->m_FenceEvent);
		for (int i = 0; i < m_dx12Setup->m_NumFrames; ++i)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			m_dx12Setup->m_BackBuffers[i].Reset();
			m_dx12Setup->m_FrameFenceValues[i] = m_dx12Setup->m_FrameFenceValues[m_dx12Setup->m_CurrentBackBufferIndex];
		}
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ThrowIfFailed(m_dx12Setup->m_SwapChain->GetDesc(&swapChainDesc));
		ThrowIfFailed(m_dx12Setup->m_SwapChain->ResizeBuffers(m_dx12Setup->m_NumFrames, m_dx12Setup->m_ClientWidth, m_dx12Setup->m_ClientHeight,
			swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

		m_dx12Setup->m_CurrentBackBufferIndex = m_dx12Setup->m_SwapChain->GetCurrentBackBufferIndex();

		m_dx12Setup->UpdateRenderTargetViews(m_dx12Setup->GetDevice(), m_dx12Setup->m_SwapChain, m_dx12Setup->m_RTVDescriptorHeap);
	}

}

void DX12Renderer::SetFullScreen(bool fullscreen)
{
	if (m_dx12Setup->m_Fullscreen != fullscreen)
	{
		m_dx12Setup->m_Fullscreen = fullscreen;

		if (m_dx12Setup->m_Fullscreen) // Switching to fullscreen.
		{
			// Store the current window dimensions so they can be restored 
			// when switching out of fullscreen state.
			::GetWindowRect(m_dx12Setup->m_hWnd, &m_dx12Setup->m_WindowRect);
			// Set the window style to a borderless window so the client area fills
// the entire screen.
			UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

			::SetWindowLongW(m_dx12Setup->m_hWnd, GWL_STYLE, windowStyle);
			// Query the name of the nearest display device for the window.
// This is required to set the fullscreen dimensions of the window
// when using a multi-monitor setup.
			HMONITOR hMonitor = ::MonitorFromWindow(m_dx12Setup->m_hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);
			::SetWindowPos(m_dx12Setup->m_hWnd, HWND_TOP,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_dx12Setup->m_hWnd, SW_MAXIMIZE); //show window in max state
		}
		else
		{
			// Restore all the window decorators.
			::SetWindowLong(m_dx12Setup->m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			::SetWindowPos(m_dx12Setup->m_hWnd, HWND_NOTOPMOST,
				m_dx12Setup->m_WindowRect.left,
				m_dx12Setup->m_WindowRect.top,
				m_dx12Setup->m_WindowRect.right - m_dx12Setup->m_WindowRect.left,
				m_dx12Setup->m_WindowRect.bottom - m_dx12Setup->m_WindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_dx12Setup->m_hWnd, SW_NORMAL);
		}
	}
	

}
