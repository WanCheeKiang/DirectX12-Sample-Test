#include"FilesInclude.h"
#include "FileWriter.h"

using namespace std;
using namespace DirectX;


void FileWriter::WriteMeshBinFile(const char* filename, Mesh* mesh)
{
	ofstream file(filename, ios::out | ios::binary);
	if (file.is_open())
	{
		uint32_t vertexCount = mesh->vertices.size();
		uint32_t indexCount = mesh->indices.size();
		uint8_t textureCount = mesh->textures.size();


		file.write((const char*)&vertexCount, sizeof(vertexCount));
		file.write((const char*)&indexCount, sizeof(indexCount));
		file.write((const char*)&textureCount, sizeof(textureCount));

		file.write((const char*)mesh->vertices.data(), sizeof(Vertex)*vertexCount);
		file.write((const char*)mesh->indices.data(), sizeof(uint32_t)*indexCount);

		for (uint8_t i = 0; i < textureCount; ++i)
			file.write((const char*)mesh->textures[i].data(), file_path_size);
		file.close();
	}
}

void FileWriter::WriteJointDataFile(const char * filename, JointData* mesh)
{
	ofstream file(filename, ios::out | ios::binary);
	if (file.is_open())
	{
		uint32_t jointCount = mesh->joints.size();
		uint32_t keyframeCount = mesh->animclip.frames.size();
		

		file.write((const char*)&jointCount, sizeof(jointCount));
		file.write((const char*)&mesh->animclip.duration, sizeof(double));
		file.write((const char*)&keyframeCount, sizeof(keyframeCount));

		file.write((const char*)mesh->joints.data(), sizeof(Joint)*jointCount);
		for (int i = 0; i < keyframeCount; ++i)
		{
			file.write((const char*)&mesh->animclip.frames[i].time, sizeof(double));

			file.write((const char*)mesh->animclip.frames[i].joints.data(), sizeof(Transform)*jointCount);
		}
		file.write((const char*)&mesh->animclip, sizeof(AnimationClip));

		file.close();
	}
}

void FileWriter::WriteSkinMeshFile(const char * filename, Mesh * mesh)
{
	ofstream file(filename, ios::out | ios::binary);
	if (file.is_open())
	{
		//uint32_t vertexCount = mesh->vertices.size();
		uint32_t indexCount = mesh->indices.size();
		uint8_t textureCount = mesh->textures.size();
		uint32_t skinVertCount = mesh->skinnedVertices.size();

		//file.write((const char*)&vertexCount, sizeof(vertexCount));
		file.write((const char*)&indexCount, sizeof(indexCount));
		file.write((const char*)&textureCount, sizeof(textureCount));
		file.write((const char*)&skinVertCount, sizeof(skinVertCount));

		//file.write((const char*)mesh->vertices.data(), sizeof(Vertex)*vertexCount);
		file.write((const char*)mesh->indices.data(), sizeof(uint32_t)*indexCount);

		for (uint8_t i = 0; i < textureCount; ++i)
			file.write((const char*)mesh->textures[i].data(), file_path_size);

		file.write((const char*)mesh->skinnedVertices.data(), sizeof(SkinnedVertex)*skinVertCount);

		
		file.close();
	}

}
