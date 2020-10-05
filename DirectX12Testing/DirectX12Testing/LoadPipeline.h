#pragma once
#include"DX12Setup.h"
#include"Renderer.h"
class LoadPipeline
{
protected:
	// Window callback function.
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	 static void ParseCommandLineArguments();
	 static void EnableDebugLayer();
	 static void RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName);
     static void Load(DX12Setup* dxsetup, HINSTANCE hInstance);
};
