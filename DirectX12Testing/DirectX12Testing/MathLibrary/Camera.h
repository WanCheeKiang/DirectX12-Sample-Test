#pragma once

#include "../DX12Setup.h"
#include "Transform.h"
struct Camera
{
	DirectX::XMMATRIX projection;
	FTransform transform;

	DirectX::XMMATRIX View()
	{
		return DirectX::XMMatrixInverse(nullptr, transform.CreateMatrix());
	}
};