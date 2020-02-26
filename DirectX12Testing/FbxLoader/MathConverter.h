#pragma once
#include"pch.h"
#include<DirectXMath.h>
namespace MathConvert
{
	DirectX::XMVECTOR FbxVec4toDxVec(FbxVector4* fbxVec);
	DirectX::XMMATRIX FbxMtx4toDxMtx(FbxMatrix* fbxVec);
}