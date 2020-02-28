#pragma once
#include"FilesInclude.h"
#include<DirectXMath.h>
namespace MathConvert
{
	DirectX::XMVECTOR FbxVec4toDxVec(FbxVector4* fbxVec);
	DirectX::XMMATRIX FbxMtx4toDxMtx(FbxMatrix* fbxVec);
}