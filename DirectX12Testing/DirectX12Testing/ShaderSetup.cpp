#include "ShaderSetup.h"

void ShaderSetup::SetupShader(ID3D12Device* device, ID3D12PipelineState* pipelineState, DX12Setup* setup, LPCWSTR vAssetName, LPCWSTR pAssetName, const char* vShaderModel, const char*  pShaderModel)
{

    
#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(vAssetName).c_str(), nullptr, nullptr, "main", vShaderModel, compileFlags, 0, &vertexShader, nullptr));
    // v this in set pixel shader function!!!! also write get set for blob(pshader&vshader)
    ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(pAssetName).c_str(), nullptr, nullptr, "main", pShaderModel, compileFlags, 0, &pixelShader, nullptr));


    //temp!!!!!! need to modify to be flexible to use different inputlayout for different type of vertex shader //example: fullsail final project (dx11) 
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };


    //create PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature  = rf.GetRootSignature().Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    
    ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));


}

void ShaderSetup::CreateVertexBuffer()
{
    const UINT vertexBufferSize = 0;
}
