#pragma once
#include"pch.h"
#include<DirectXMath.h>
struct Vertex
{

	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT3 normals;
	DirectX::XMFLOAT2 texCoords;
	
};

struct SkinnedVertex
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT3 normals;
	DirectX::XMFLOAT2 texCoords;
	DirectX::XMFLOAT4 weights;
	DirectX::XMINT4 indexs;
};
struct FbxJoint
{
	FbxNode* node; 
	int parentIndex;
};

struct Transform
{
	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR scale;
	DirectX::XMMATRIX createMatrix() const
	{
		return DirectX::XMMatrixScalingFromVector(scale)*DirectX::XMMatrixRotationQuaternion(rotation)*DirectX::XMMatrixTranslationFromVector(pos);
	}

	void setMatrix(const DirectX::XMMATRIX& mtx)
	{
		XMMatrixDecompose(&scale, &rotation, &pos, mtx);
	}

	Transform()
	{
		pos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		rotation = DirectX::XMQuaternionIdentity();
		scale = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

	}

	DirectX::XMVECTOR Forward()
	{
		DirectX::XMVECTOR dir = DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f);
		DirectX::XMVECTOR direction = DirectX::XMVector3Rotate(dir, rotation);

		return direction;
	}

	DirectX::XMVECTOR Right()
	{
		DirectX::XMVECTOR dir = DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f);
		DirectX::XMVECTOR direction = DirectX::XMVector3Rotate(dir, rotation);

		return direction;
	}

	DirectX::XMVECTOR Up()
	{
		DirectX::XMVECTOR dir = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
		DirectX::XMVECTOR direction = DirectX::XMVector3Rotate(dir, rotation);

		return direction;
	}
};

struct Joint
{
	//DirectX::XMMATRIX form;
	Transform trans;
	int parentIndex;
};

struct KeyFrame
{
	double time;
	std::vector<Transform> joints;
};

struct AnimationClip
{
	double duration; 
	std::vector<KeyFrame> frames;
};

struct JointData
{
	std::vector<Joint> joints;
	AnimationClip animclip;

};

struct Influence
{
	int joint = 0;
	float weight = 0;
};
constexpr uint32_t file_path_size = 260;
using file_path_t = std::array<char, file_path_size>;

struct Mesh
{
	//vector of vertices
	std::vector<Vertex> vertices;
	//vector of indices
	std::vector<uint32_t> indices;
	//vector of texture paths
	std::vector<file_path_t> textures;

	std::vector<SkinnedVertex> skinnedVertices;
};

struct Material
{
	enum e_component { EMISSIVE = 0, DIFFUSE, SPECULAR, SHININESS, COUNT };
	struct component_t
	{
		float value[3] = { 0.0f, 0.0f, 0.0f };
		float factor = 0.0f;
		int64_t input = -1;
	};
	component_t& operator[](int i) { return components[i]; }
	const component_t& operator[](int i)const { return components[i]; }
private:
	component_t components[COUNT];
};

