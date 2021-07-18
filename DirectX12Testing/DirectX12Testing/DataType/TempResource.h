#pragma once
//temp file name, need to rename. TODO
#include"../DX12Setup.h"

//for e.g CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD) 

namespace HeapProperties
{
	const CD3DX12_HEAP_PROPERTIES Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_HEAP_PROPERTIES Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
};

//temp placement 
struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	DirectX::XMFLOAT4 outputColor;
	DirectX::XMFLOAT4 offset;
};