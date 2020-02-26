#include "pch.h"
#include "MathConverter.h"
using namespace DirectX;

XMVECTOR MathConvert::FbxVec4toDxVec(FbxVector4*  fbxVec)
{
	XMVECTOR output;
	output = XMVectorSet((*fbxVec)[0], (*fbxVec)[1], (*fbxVec)[2], (*fbxVec)[3]);

	return output;
}

DirectX::XMMATRIX MathConvert::FbxMtx4toDxMtx(FbxMatrix* fbxVec)
{	
	XMMATRIX output;
	for (int i = 0; i < 4; ++i)
	{
		FbxVector4 row = fbxVec->GetRow(i);
		output.r[i] = FbxVec4toDxVec(&row);
	}
	return output;
}
