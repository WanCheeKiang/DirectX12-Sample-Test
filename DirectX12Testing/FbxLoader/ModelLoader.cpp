// ModelLoader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "FileWriter.h"
#include "FBXLoader.h"

using namespace std;

int main()
{
	std::string name;
	getline(cin, name);

	Mesh model;
	JointData jointdata;

	string fbxName = name + ".fbx";
	if (ImportFbxModel(fbxName.c_str(), model,jointdata))
	{
		string meshName = name + ".bin";
		string jointName = name + ".joint";
		string skinMeshName = name + ".skinmesh";

		//FileWriter::WriteMeshBinFile(meshName.c_str(), &model);
		FileWriter::WriteJointDataFile(jointName.c_str(), &jointdata);
		FileWriter::WriteSkinMeshFile(skinMeshName.c_str(), &model);
	}
}
