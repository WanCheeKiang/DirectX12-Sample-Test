#include "LoadPipeline.h"
#include "KeyInput.h"


LRESULT LoadPipeline::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DX12Setup* dx12Setup = DX12Setup::GetSetup();
	DX12Renderer* dx12Redner = DX12Renderer::GetRenderer();
	if (dx12Setup->m_IsInitialized)
	{
		switch (message)
		{
		case WM_PAINT:
			dx12Redner->Update();
			dx12Redner->Render();
			break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

			switch (wParam)
			{
			case 'V':
				dx12Setup->m_VSync = !dx12Setup->m_VSync;
				break;
			case VK_ESCAPE:
				::PostQuitMessage(0);
				break;
			case VK_RETURN:
				if (alt)
				{
			case VK_F11:
				//if(KeyInput::GetKeyState(KeyCode::F) == KeyState::Down)
				dx12Redner->SetFullScreen(!dx12Setup->m_Fullscreen);

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
				::GetClientRect(dx12Setup->m_hWnd, &clientRect);

				int width = clientRect.right - clientRect.left;
				int height = clientRect.bottom - clientRect.top;

				dx12Redner->Resize(width, height);
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

void LoadPipeline::ParseCommandLineArguments()
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

void LoadPipeline::EnableDebugLayer()
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

void LoadPipeline::RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName)
{
	// Register a window class for creating our render window with.
	WNDCLASSEXW windowClass = {};

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
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

void LoadPipeline::Load(DX12Setup* dx12setup, HINSTANCE hInstance)
{
	// Window class name. Used for registering / creating the window.
	const wchar_t* windowClassName = L"DX12WindowClass";
	ParseCommandLineArguments();
	EnableDebugLayer();
	dx12setup->m_TearingSupported = dx12setup->CheckTearingSupport();

	RegisterWindowClass(hInstance, windowClassName);
	dx12setup->m_hWnd = dx12setup->CreateWindow(windowClassName, hInstance, L"Learning DirectX 12",
		dx12setup->m_ClientWidth, dx12setup->m_ClientHeight);

	// Initialize the global window rect variable.
	::GetWindowRect(dx12setup->m_hWnd, &dx12setup->m_WindowRect);
	ComPtr<IDXGIAdapter4> dxgiAdapter4 = dx12setup->GetAdapter(dx12setup->m_UseWarp);

	//dx12setup->GetDevice() = dx12setup->CreateDevice(dxgiAdapter4);
	dx12setup->SetDevice(dx12setup->CreateDevice(dxgiAdapter4));

	dx12setup->m_CommandQueue = dx12setup->CreateCommandQueue(dx12setup->GetDevice(), D3D12_COMMAND_LIST_TYPE_DIRECT);

	dx12setup->m_SwapChain = dx12setup->CreateSwapChain(dx12setup->m_hWnd, dx12setup->m_CommandQueue,
		dx12setup->m_ClientWidth, dx12setup->m_ClientHeight, dx12setup->m_NumFrames);

	dx12setup->m_CurrentBackBufferIndex = dx12setup->m_SwapChain->GetCurrentBackBufferIndex();

	dx12setup->m_RTVDescriptorHeap = dx12setup->CreateDescriptorHeap(dx12setup->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, dx12setup->m_NumFrames);
	dx12setup->m_RTVDescriptorSize = dx12setup->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	dx12setup->UpdateRenderTargetViews(dx12setup->GetDevice(), dx12setup->m_SwapChain, dx12setup->m_RTVDescriptorHeap);
	for (int i = 0; i < dx12setup->m_NumFrames; ++i)
	{
		dx12setup->m_CommandAllocators[i] = dx12setup->CreateCommandAllocator(dx12setup->GetDevice(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	}
	dx12setup->m_CommandList = dx12setup->CreateCommandList(dx12setup->GetDevice(),
		dx12setup->m_CommandAllocators[dx12setup->m_CurrentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);

	dx12setup->m_Fence = dx12setup->CreateFence(dx12setup->GetDevice());
	dx12setup->m_FenceEvent = dx12setup->CreateEventHandle();
	dx12setup->m_IsInitialized = true;

	::ShowWindow(dx12setup->m_hWnd, SW_SHOW);
}