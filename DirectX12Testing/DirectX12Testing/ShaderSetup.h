#pragma once
#include<vector>
#include"DX12Setup.h"
class ShaderSetup
{
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
	//create vertex shader & pixel shader 

	void SetupVertexShader(ID3D12Device device, ID3DBlob blob, ID3D12PipelineState pipelineState);

	void SetupPixelShader();

};