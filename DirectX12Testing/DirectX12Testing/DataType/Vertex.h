#pragma once
#include<DirectXMath.h>
#include"..//MathLibrary/MathConstants.h"
namespace Vertex
{
	struct BaseVertex
	{
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT4 m_color;
		DirectX::XMFLOAT2 m_uv;
		DirectX::XMFLOAT3 m_normal;
		DirectX::XMFLOAT3 m_tangent;
		DirectX::XMFLOAT3 m_binormal;

		BaseVertex();
		BaseVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color);
        inline bool operator==(const BaseVertex& rhs) const
        {
            const BaseVertex& lhs = *this;

            bool output = (fabsf(lhs.m_position.x - rhs.m_position.x) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_position.y - rhs.m_position.y) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_position.z - rhs.m_position.z) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_position.x - rhs.m_normal.x) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_normal.y - rhs.m_normal.y) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_normal.z - rhs.m_normal.z) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_uv.x - rhs.m_uv.x) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_uv.y - rhs.m_uv.y) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_color.x - rhs.m_color.x) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_color.y - rhs.m_color.y) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_color.z - rhs.m_color.z) < MathConstants::kEpsilon) &&
                (fabsf(lhs.m_color.w - rhs.m_color.w) < MathConstants::kEpsilon);
            return output;
        };


	};

}