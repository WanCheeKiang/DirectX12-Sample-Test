// DirectX12Testing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//This project's tutorial is from  https://www.3dgep.com/, github:https://github.com/jpvanoosten/LearningDirectX12
//This project code is NOT completely copying the tutorial
#include<Windows.h>
#include <iostream>
#include"Renderer.h"
#include "LoadPipeline.h"
#include "KeyInput.h"
DX12Setup* g_dx12Setup = DX12Setup::GetSetup();
DX12Renderer* g_dx12Redner = DX12Renderer::GetRenderer();

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	bool close = false;
	// Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
	// Using this awareness context allows the client area of the window 
	// to achieve 100% scaling while still allowing non-client window content to 
	// be rendered in a DPI sensitive fashion.
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	//key input
	KeyInput::Init();

	LoadPipeline::Load(g_dx12Setup, hInstance);
	
	MSG msg = {};


	while (msg.message != WM_QUIT)
	{
		KeyInput::Update();

		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	// Make sure the command queue has finished all commands before closing.
	g_dx12Setup->Flush(g_dx12Setup->m_CommandQueue, g_dx12Setup->m_Fence, g_dx12Setup->m_FenceValue, g_dx12Setup->m_FenceEvent);

	::CloseHandle(g_dx12Setup->m_FenceEvent);

	return 0;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
