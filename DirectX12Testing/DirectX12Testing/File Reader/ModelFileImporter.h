#pragma once 
#include"..//HeaderIncludes.h"
#include"..//DataType/FVertex.h"
constexpr uint32_t file_path_size = 260;
using file_path_t = std::array<char, file_path_size>;
namespace FileImport
{
	namespace Import
	{
		struct MeshImport
		{
			//vector of vertices
			std::vector<FVertex::Vertex> vertices;
			//vector of indices
			std::vector<uint32_t> indices;
			//vector of texture paths
			std::vector<file_path_t> textures;

		};

		struct SkinnedMeshImport
		{
			//std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			std::vector<file_path_t> textures;
			//std::vector<SkinnedVertex> skinmesh;
		};
		bool ReadBinFile(const char* filename, MeshImport* mesh);
		//bool ReadJointFile(const char* filename, JointImport* mesh);
		//bool ReadSkinnedmeshFile(const char* filename, SkinnedMeshImport* skinMesh);
	}
}