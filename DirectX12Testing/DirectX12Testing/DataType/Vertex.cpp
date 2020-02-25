#include "Vertex.h"

Vertex::BaseVertex::BaseVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color)
{
	m_position = pos;
	m_color = color;
}
