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
	
	// temp helper function //from dx12samples
	
	// Root assets path.
	std::wstring m_assetsPath;
	// Helper function for resolving the full path of assets.
	std::wstring GetAssetFullPath(LPCWSTR assetName)
	{
		return m_assetsPath + assetName;
	}

	//create vertex shader & pixel shader 

	void SetupShader(ID3D12Device* device, ID3D12PipelineState* pipelineState, DX12Setup* setup, LPCWSTR vAssetName, LPCWSTR pAssetName, const char* vShaderModel, const char* pShaderModelS);

	void CreateVertexBuffer();

};