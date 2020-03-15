#pragma once
#include<DirectXMath.h>
#include"FVertex.h"
#include"..//MathLibrary/Transform.h"
struct Camera
{
	DirectX::XMMATRIX projection;
	FTransform transform;
	DirectX::XMMATRIX View()
	{
		return DirectX::XMMatrixInverse(nullptr, transform.CreateMatrix());
	}
};