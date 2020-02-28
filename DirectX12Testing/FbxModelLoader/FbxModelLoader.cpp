// FbxModelLoader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "FilesInclude.h"
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
	if (ImportFbxModel(fbxName.c_str(), model, jointdata))
	{
		string meshName = name + ".bin";
		string jointName = name + ".joint";
		string skinMeshName = name + ".skinmesh";

		//FileWriter::WriteMeshBinFile(meshName.c_str(), &model);
		FileWriter::WriteJointDataFile(jointName.c_str(), &jointdata);
		FileWriter::WriteSkinMeshFile(skinMeshName.c_str(), &model);
	}
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
