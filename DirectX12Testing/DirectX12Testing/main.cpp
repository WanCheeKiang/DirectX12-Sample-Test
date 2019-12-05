// DirectX12Testing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//This project's tutorial is from  https://www.3dgep.com/, github:https://github.com/jpvanoosten/LearningDirectX12
//This project code is NOT completely copying the tutorial
#include<Windows.h>
#include <iostream>
#include"DX12Setup.h"
#include"Renderer.h"
#include"KeyInput.h"

DX12Setup* g_dx12Setup = DX12Setup::GetSetup();
DX12Renderer* g_dx12Redner = DX12Renderer::GetRenderer();
// Window callback function.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_dx12Setup->m_IsInitialized)
	{
		switch (message)
		{
		case WM_PAINT:
			g_dx12Redner->Update();
			g_dx12Redner->Render();
			break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

			switch (wParam)
			{
			case 'V':
				g_dx12Setup->m_VSync = !g_dx12Setup->m_VSync;
				break;
			case VK_ESCAPE:
				::PostQuitMessage(0);
				break;
			case VK_RETURN:
				if (alt)
				{
			case VK_F11:
					//if(KeyInput::GetKeyState(KeyCode::F) == KeyState::Down)
				g_dx12Redner->SetFullScreen(!g_dx12Setup->m_Fullscreen);
			
				}
				break;
			}
		}
		break;
		case WM_INPUT:
			KeyInput::WinProc(message, wParam, lParam);
			break;
			// The default window procedure will play a system notification sound 
			// when pressing the Alt+Enter keyboard combination if this message is 
			// not handled.
		case WM_SYSCHAR:
			break;
			{
				RECT clientRect = {};
				::GetClientRect(g_dx12Setup->m_hWnd, &clientRect);

				int width = clientRect.right - clientRect.left;
				int height = clientRect.bottom - clientRect.top;

				g_dx12Redner->Resize(width, height);
			}
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		default:
			return ::DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}
	else
	{
		return ::DefWindowProcW(hwnd, message, wParam, lParam);
	}

	return 0;

}

void RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName)
{
	// Register a window class for creating our render window with.
	WNDCLASSEXW windowClass = {};

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInst;
	windowClass.hIcon = ::LoadIcon(hInst, NULL);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = ::LoadIcon(hInst, NULL);

	static ATOM atom = ::RegisterClassExW(&windowClass);
	assert(atom > 0);
}

void ParseCommandLineArguments()
{

	int argc;
	wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);

	for (size_t i = 0; i < argc; ++i)
	{
		if (::wcscmp(argv[i], L"-w") == 0 || ::wcscmp(argv[i], L"--width") == 0)
		{
			DX12Setup::GetSetup()->m_ClientWidth = ::wcstol(argv[++i], nullptr, 10);
		}
		if (::wcscmp(argv[i], L"-h") == 0 || ::wcscmp(argv[i], L"--height") == 0)
		{
			DX12Setup::GetSetup()->m_ClientHeight = ::wcstol(argv[++i], nullptr, 10);
		}
		if (::wcscmp(argv[i], L"-warp") == 0 || ::wcscmp(argv[i], L"--warp") == 0)
		{
			DX12Setup::GetSetup()->m_UseWarp = true;
		}
	}

	// Free memory allocated by CommandLineToArgvW
	::LocalFree(argv);
}

void EnableDebugLayer()
{
#if defined(_DEBUG)
	// Always enable the debug layer before doing anything DX12 related
	// so all possible errors generated while creating DX12 objects
	// are caught by the debug layer.
	ComPtr<ID3D12Debug> debugInterface;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
#endif
}


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
	// Window class name. Used for registering / creating the window.
	const wchar_t* windowClassName = L"DX12WindowClass";
	ParseCommandLineArguments();
	EnableDebugLayer();
	g_dx12Setup->m_TearingSupported = g_dx12Setup->CheckTearingSupport();

	RegisterWindowClass(hInstance, windowClassName);
	g_dx12Setup->m_hWnd = g_dx12Setup->CreateWindow(windowClassName, hInstance, L"Learning DirectX 12",
		g_dx12Setup->m_ClientWidth, g_dx12Setup->m_ClientHeight);

	// Initialize the global window rect variable.
	::GetWindowRect(g_dx12Setup->m_hWnd, &g_dx12Setup->m_WindowRect);
	ComPtr<IDXGIAdapter4> dxgiAdapter4 = g_dx12Setup->GetAdapter(g_dx12Setup->m_UseWarp);

	//g_dx12Setup->GetDevice() = g_dx12Setup->CreateDevice(dxgiAdapter4);
	g_dx12Setup->SetDevice(g_dx12Setup->CreateDevice(dxgiAdapter4));

	g_dx12Setup->m_CommandQueue = g_dx12Setup->CreateCommandQueue(g_dx12Setup->GetDevice(), D3D12_COMMAND_LIST_TYPE_DIRECT);

	g_dx12Setup->m_SwapChain = g_dx12Setup->CreateSwapChain(g_dx12Setup->m_hWnd, g_dx12Setup->m_CommandQueue,
		g_dx12Setup->m_ClientWidth, g_dx12Setup->m_ClientHeight, g_dx12Setup->m_NumFrames);

	g_dx12Setup->m_CurrentBackBufferIndex = g_dx12Setup->m_SwapChain->GetCurrentBackBufferIndex();

	g_dx12Setup->m_RTVDescriptorHeap = g_dx12Setup->CreateDescriptorHeap(g_dx12Setup->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, g_dx12Setup->m_NumFrames);
	g_dx12Setup->m_RTVDescriptorSize = g_dx12Setup->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	g_dx12Setup->UpdateRenderTargetViews(g_dx12Setup->GetDevice(), g_dx12Setup->m_SwapChain, g_dx12Setup->m_RTVDescriptorHeap);
	for (int i = 0; i < g_dx12Setup->m_NumFrames; ++i)
	{
		g_dx12Setup->m_CommandAllocators[i] = g_dx12Setup->CreateCommandAllocator(g_dx12Setup->GetDevice(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	}
	g_dx12Setup->m_CommandList = g_dx12Setup->CreateCommandList(g_dx12Setup->GetDevice(),
		g_dx12Setup->m_CommandAllocators[g_dx12Setup->m_CurrentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);

	g_dx12Setup->m_Fence = g_dx12Setup->CreateFence(g_dx12Setup->GetDevice());
	g_dx12Setup->m_FenceEvent = g_dx12Setup->CreateEventHandle();
	g_dx12Setup->m_IsInitialized = true;

	::ShowWindow(g_dx12Setup->m_hWnd, SW_SHOW);


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

//int main()
//{
//	std::cout << "Hello World!\n";
//}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
