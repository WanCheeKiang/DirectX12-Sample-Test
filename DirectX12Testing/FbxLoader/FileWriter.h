#pragma once
#include"ModelInfo.h"
namespace FileWriter
{
	void WriteMeshBinFile(const char* filename, Mesh* mesh);
	void WriteJointDataFile(const char* filename, JointData* jointdata);
	void WriteSkinMeshFile(const char* filename, Mesh* mesh);
};

