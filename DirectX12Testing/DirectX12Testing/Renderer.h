#pragma once
#include"DX12Setup.h"
#include"MathLibrary/MathLibrary.h"
//#include <wrl/client.h>
//Microsoft::WRL::ComPtr<>
class DX12Renderer
{
	DX12Setup* m_dx12Setup = DX12Setup::GetSetup();
	//temp
	DirectX::XMMATRIX viewMtx;
	DirectX::XMMATRIX projMtx;
	ID3D12PipelineState* m_pipelineStateObject; // pso containing a pipeline state

	ID3D12RootSignature* m_rootSignature; // root signature defines data shaders will access

	D3D12_VIEWPORT m_viewport; // area that output from rasterizer will be stretched to.

	D3D12_RECT m_scissorRect; // the area to draw in. pixels outside that area will not be drawn onto
	
public:
	static DX12Renderer* GetRenderer();
	//renderer function and update
	void Update();
	void Render();
	void Resize(uint32_t width, uint32_t height);
	void SetFullScreen(bool fullscreen);


};
