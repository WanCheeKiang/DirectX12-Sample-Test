#pragma once
#include<vector>
#include"DX12Setup.h"
#include"Renderer.h"
#include"RendererFunction.h"
class ShaderSetup
{
	RednererFunction rf; // temp for getting the root signature
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
	
	//create vertex shader & pixel shader 

	void SetupVertexShader(ID3D12Device* device, ID3D12PipelineState* pipelineState, DX12Setup* setup);

	void SetupPixelShader();

};