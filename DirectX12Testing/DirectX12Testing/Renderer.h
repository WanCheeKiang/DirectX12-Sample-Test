#pragma once
#include"DX12Setup.h"
#include"MathLibrary/MathLibrary.h"

class DX12Renderer
{
	DX12Setup* m_dx12Setup = DX12Setup::GetSetup();
	DirectX::XMMATRIX viewMtx;
	DirectX::XMMATRIX projMtx;
public:
	static DX12Renderer* GetRenderer();
	//renderer function and update
	void Update();
	void Render();
	void Resize(uint32_t width, uint32_t height);
	void SetFullScreen(bool fullscreen);


};
