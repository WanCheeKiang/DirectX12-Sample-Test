#include "FBXLoader.h"
#include"MathConverter.h"
#include <string.h>

using namespace std;
using namespace MathConvert;

const int MaxInfluence = 4;
using influenceSet = array<Influence, MaxInfluence>;

void Compactify(int numIndices, int numVertices, Vertex* vertices, int* indices, Mesh& model)
{
	float epsilon = 0.1001f;
	//check if thers indices and vertices are repeated
	for (int i = 0; i < numIndices; i++)
	{
		bool found = false;
		unsigned int j;
		for (j = 0; j < model.vertices.size(); j++)
		{
			if (abs(vertices[indices[i]].pos.x - model.vertices[j].pos.x) < epsilon	&&
				abs(vertices[indices[i]].pos.y - model.vertices[j].pos.y) < epsilon	&&
				abs(vertices[indices[i]].pos.z - model.vertices[j].pos.z) < epsilon	&&
				vertices[indices[i]].normals.x == model.vertices[j].normals.x	&&
				vertices[indices[i]].normals.y == model.vertices[j].normals.y	&&
				vertices[indices[i]].normals.z == model.vertices[j].normals.z &&
				vertices[indices[i]].texCoords.x == model.vertices[j].texCoords.x    &&
				vertices[indices[i]].texCoords.y == model.vertices[j].texCoords.y)
			{
				found = true;
				break;
			}

		}
		if (found == true)
		{
			indices[i] = j;
		}
		else
		{
			model.vertices.push_back(vertices[indices[i]]);
			indices[i] = model.vertices.size() - 1;
		}
	}

	delete vertices;
	model.indices.resize(numIndices);
	for (int i = 0; i < numIndices; i++)
	{
		model.indices[i] = indices[i];
	}
	numVertices = model.vertices.size();
	// print out some stats
}

void ProcessFbxMesh(FbxNode* Node, Mesh* Model)
{

	//FBX Mesh stuff
	int childrenCount = Node->GetChildCount();

	// Get the Normals array from the mesh
	FbxArray<FbxVector4> normalsVec;

	for (int i = 0; i < childrenCount; i++)
	{
		FbxNode *childNode = Node->GetChild(i);
		FbxMesh *mesh = childNode->GetMesh();

		if (mesh != NULL)
		{
			// Get index count from mesh
			int numIndices = mesh->GetPolygonVertexCount();

			// No need to allocate int array, FBX does for us
			int* indices = mesh->GetPolygonVertices();

			// Get vertex count from mesh
			int numVertices = mesh->GetControlPointsCount();

			mesh->GetPolygonVertexNormals(normalsVec);

			// Create SimpleVertex array to size of this mesh
			Vertex* vertices = new Vertex[numVertices];

			FbxStringList UvNameList;
			mesh->GetUVSetNames(UvNameList);
			FbxVector2 UVvalue2;


			//================= Process Vertices ===================
			for (int j = 0; j < numVertices; j++)
			{
				FbxVector4 vert = mesh->GetControlPointAt(j);
				vertices[j].pos.x = (float)vert.mData[0];
				vertices[j].pos.y = (float)vert.mData[1];
				vertices[j].pos.z = (float)vert.mData[2];
			}
			// Declare a new array for the second vertex array
			// Note the size is numIndices not numVertices
			Vertex *vertices2 = new Vertex[numIndices];
			// align (expand) vertex array and set the normals
			for (int j = 0; j < numIndices; j++)
			{
				vertices2[j].pos = vertices[indices[j]].pos;
				vertices2[j].normals.x = (float)normalsVec[j][0];
				vertices2[j].normals.y = (float)normalsVec[j][1];
				vertices2[j].normals.z = (float)normalsVec[j][2];
			}

			for (int UVindex = 0; UVindex < UvNameList.GetCount(); UVindex++)
			{
				const char* UvSetName = UvNameList.GetStringAt(UVindex);
				FbxGeometryElementUV* UVelement = mesh->GetElementUV(UvSetName);
				if (!UVelement)
					continue;
				if (UVelement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex&&
					UVelement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
					return;
				const bool useIndex = UVelement->GetReferenceMode() != FbxGeometryElement::eDirect;
				const int IndexCount = (useIndex) ? UVelement->GetIndexArray().GetCount() : 0;

				const int PolyCount = mesh->GetPolygonCount();

				if (UVelement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					for (int PolyIndex = 0; PolyIndex < PolyCount; PolyIndex++)
					{
						const int PolySize = mesh->GetPolygonSize(PolyIndex);
						for (int vertIndex = 0; vertIndex < PolySize; vertIndex++)
						{
							FbxVector2 UVvalue;
							int PolyVertIndex = mesh->GetPolygonVertex(PolyIndex, vertIndex);
							int UVindex = useIndex ? UVelement->GetIndexArray().GetAt(PolyVertIndex) : PolyVertIndex;
							UVvalue = UVelement->GetDirectArray().GetAt(UVindex);
							vertices2[PolyIndex].texCoords.x = (float)UVvalue.mData[0];
							vertices2[PolyIndex].texCoords.y = 1.0f - (float)UVvalue.mData[1];
						}
					}
				}
				else if (UVelement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					int PolyIndexCounter = 0;
					for (int PolyIndex = 0; PolyIndex < PolyCount; ++PolyIndex)
					{
						// build the max index array that we need to pass into MakePoly
						const int PolySize = mesh->GetPolygonSize(PolyIndex);
						for (int VertIndex = 0; VertIndex < PolySize; ++VertIndex)
						{
							if (PolyIndexCounter < IndexCount)
							{

								//the UV index depends on the reference mode
								int lUVIndex = useIndex ? UVelement->GetIndexArray().GetAt(PolyIndexCounter) : PolyIndexCounter;

								UVvalue2 = UVelement->GetDirectArray().GetAt(lUVIndex);
								vertices2[PolyIndexCounter].texCoords.x = (float)UVvalue2.mData[0];
								vertices2[PolyIndexCounter].texCoords.y = 1.0f - (float)UVvalue2.mData[1];

								PolyIndexCounter++;
							}
						}
					}
				}

			}

			// vertices is an "out" var so make sure it points to the new array
			// and clean up first array
			delete vertices;
			vertices = vertices2;

			// make new indices to match the new vertex(2) array
			delete indices;
			indices = new int[numIndices];
			for (int j = 0; j < numIndices; j++)
			{
				indices[j] = j;
			}

			if (true)
			{

				Compactify(numIndices, numVertices, vertices, indices, *Model);
			}

			else
			{
				// numVertices is an "out" var so set to new size
				// this is used in the DrawIndexed functions to set the 
				// the right number of triangles
				numVertices = numIndices;
			}

			ProcessFbxMesh(childNode, Model);

			delete[] indices;
		}
	}
}
void SkinCompactify(int numIndices, int numVertices, SkinnedVertex* vertices, int* indices, Mesh& model)
{
	float epsilon = 0.1001f;
	//check if thers indices and vertices are repeated
	for (int i = 0; i < numIndices; i++)
	{
		bool found = false;
		unsigned int j;
		for (j = 0; j < model.skinnedVertices.size(); j++)
		{
			if (abs(vertices[indices[i]].pos.x - model.skinnedVertices[j].pos.x) < epsilon		&&
				abs(vertices[indices[i]].pos.y - model.skinnedVertices[j].pos.y) < epsilon		&&
				abs(vertices[indices[i]].pos.z - model.skinnedVertices[j].pos.z) < epsilon		&&
				vertices[indices[i]].normals.x == model.skinnedVertices[j].normals.x			&&
				vertices[indices[i]].normals.y == model.skinnedVertices[j].normals.y			&&
				vertices[indices[i]].normals.z == model.skinnedVertices[j].normals.z			&&
				vertices[indices[i]].texCoords.x == model.skinnedVertices[j].texCoords.x		&&
				vertices[indices[i]].texCoords.y == model.skinnedVertices[j].texCoords.y		&&
				vertices[indices[i]].weights.x == model.skinnedVertices[j].weights.x	&&
				vertices[indices[i]].weights.y == model.skinnedVertices[j].weights.y	&&
				vertices[indices[i]].weights.z == model.skinnedVertices[j].weights.z	&&
				vertices[indices[i]].weights.w == model.skinnedVertices[j].weights.w	&&
				vertices[indices[i]].indexs.x == model.skinnedVertices[j].indexs.x		&&
				vertices[indices[i]].indexs.y == model.skinnedVertices[j].indexs.y		&&
				vertices[indices[i]].indexs.z == model.skinnedVertices[j].indexs.z		&&
				vertices[indices[i]].indexs.w == model.skinnedVertices[j].indexs.w)

			{
				found = true;
				break;
			}

		}
		if (found == true)
		{
			indices[i] = j;
		}
		else
		{
			model.skinnedVertices.push_back(vertices[indices[i]]);
			indices[i] = model.skinnedVertices.size() - 1;
		}
	}

	delete vertices;
	model.indices.resize(numIndices);
	for (int i = 0; i < numIndices; i++)
	{
		model.indices[i] = indices[i];
	}
	numVertices = model.skinnedVertices.size();
}
void ProcessFbxSkinnedMesh(FbxMesh* mesh, vector<influenceSet> set, Mesh* Model)
{
	// Get the Normals array from the mesh
	FbxArray<FbxVector4> normalsVec;

	if (mesh != NULL)
	{
		// Get index count from mesh
		int numIndices = mesh->GetPolygonVertexCount();

		// No need to allocate int array, FBX does for us
		int* indices = mesh->GetPolygonVertices();

		// Get vertex count from mesh
		int numVertices = mesh->GetControlPointsCount();

		mesh->GetPolygonVertexNormals(normalsVec);

		// Create SimpleVertex array to size of this mesh
		SkinnedVertex* vertices = new SkinnedVertex[numVertices];

		FbxStringList UvNameList;
		mesh->GetUVSetNames(UvNameList);
		FbxVector2 UVvalue2;


		//================= Process Vertices ===================
		for (int j = 0; j < numVertices; j++)
		{
			FbxVector4 vert = mesh->GetControlPointAt(j);
			vertices[j].pos.x = (float)vert.mData[0];
			vertices[j].pos.y = (float)vert.mData[1];
			vertices[j].pos.z = (float)vert.mData[2];
			vertices[j].indexs.x = set[j][0].joint;
			vertices[j].indexs.y = set[j][1].joint;
			vertices[j].indexs.z = set[j][2].joint;
			vertices[j].indexs.w = set[j][3].joint;
			vertices[j].weights.x = set[j][0].weight;
			vertices[j].weights.y = set[j][1].weight;
			vertices[j].weights.z = set[j][2].weight;
			vertices[j].weights.w = set[j][3].weight;
		}
		// Declare a new array for the second vertex array
		// Note the size is numIndices not numVertices
		SkinnedVertex *vertices2 = new SkinnedVertex[numIndices];
		// align (expand) vertex array and set the normals
		for (int j = 0; j < numIndices; j++)
		{
			vertices2[j] = vertices[indices[j]];
			vertices2[j].normals.x = (float)normalsVec[j][0];
			vertices2[j].normals.y = (float)normalsVec[j][1];
			vertices2[j].normals.z = (float)normalsVec[j][2];
		}

		for (int UVindex = 0; UVindex < UvNameList.GetCount(); UVindex++)
		{
			const char* UvSetName = UvNameList.GetStringAt(UVindex);
			FbxGeometryElementUV* UVelement = mesh->GetElementUV(UvSetName);
			if (!UVelement)
				continue;
			if (UVelement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex&&
				UVelement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
				return;
			const bool useIndex = UVelement->GetReferenceMode() != FbxGeometryElement::eDirect;
			const int IndexCount = (useIndex) ? UVelement->GetIndexArray().GetCount() : 0;

			const int PolyCount = mesh->GetPolygonCount();

			if (UVelement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				for (int PolyIndex = 0; PolyIndex < PolyCount; PolyIndex++)
				{
					const int PolySize = mesh->GetPolygonSize(PolyIndex);
					for (int vertIndex = 0; vertIndex < PolySize; vertIndex++)
					{
						FbxVector2 UVvalue;
						int PolyVertIndex = mesh->GetPolygonVertex(PolyIndex, vertIndex);
						int UVindex = useIndex ? UVelement->GetIndexArray().GetAt(PolyVertIndex) : PolyVertIndex;
						UVvalue = UVelement->GetDirectArray().GetAt(UVindex);
						vertices2[PolyIndex].texCoords.x = (float)UVvalue.mData[0];
						vertices2[PolyIndex].texCoords.y = 1.0f - (float)UVvalue.mData[1];
					}
				}
			}
			else if (UVelement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				int PolyIndexCounter = 0;
				for (int PolyIndex = 0; PolyIndex < PolyCount; ++PolyIndex)
				{
					// build the max index array that we need to pass into MakePoly
					const int PolySize = mesh->GetPolygonSize(PolyIndex);
					for (int VertIndex = 0; VertIndex < PolySize; ++VertIndex)
					{
						if (PolyIndexCounter < IndexCount)
						{

							//the UV index depends on the reference mode
							int lUVIndex = useIndex ? UVelement->GetIndexArray().GetAt(PolyIndexCounter) : PolyIndexCounter;

							UVvalue2 = UVelement->GetDirectArray().GetAt(lUVIndex);
							vertices2[PolyIndexCounter].texCoords.x = (float)UVvalue2.mData[0];
							vertices2[PolyIndexCounter].texCoords.y = 1.0f - (float)UVvalue2.mData[1];

							PolyIndexCounter++;
						}
					}
				}
			}

		}

		// vertices is an "out" var so make sure it points to the new array
		// and clean up first array
		delete vertices;
		vertices = vertices2;

		// make new indices to match the new vertex(2) array
		delete indices;
		indices = new int[numIndices];
		for (int j = 0; j < numIndices; j++)
		{
			indices[j] = j;
		}

		if (true)
		{

			SkinCompactify(numIndices, numVertices, vertices, indices, *Model);
		}

		else
		{
			// numVertices is an "out" var so set to new size
			// this is used in the DrawIndexed functions to set the 
			// the right number of triangles
			numVertices = numIndices;
		}


		delete[] indices;
	}
}
void GetJointsAnimationData(FbxNode* fbxNode, FbxScene* scene, JointData* mesh, Mesh* model)
{
	vector<FbxJoint> fbxJoints;
	vector<Joint> joints;
	int poseCount = scene->GetPoseCount();
	FbxPose* pose = nullptr;
	vector<influenceSet> controlPtInfluence;
	vector<FbxCluster*> fbxClusters;


	for (int i = 0; i < poseCount; ++i)
	{
		pose = scene->GetPose(i);

		for (int j = 0; j < pose->GetCount(); ++j)
		{
			if (pose->IsBindPose())
			{
				FbxNode* node = pose->GetNode(j);
				FbxSkeleton* skeleton = node->GetSkeleton();
				if (skeleton != nullptr)
				{
					if (skeleton->IsSkeletonRoot())
					{
						FbxJoint tempRoot;
						tempRoot.parentIndex = -1;
						tempRoot.node = node;
						fbxJoints.push_back(tempRoot);
						break;
					}
				}
			}

		}
	}

	if (pose == nullptr)
		return;

	for (int i = 0; i < fbxJoints.size(); ++i)
	{
		int childCount = fbxJoints[i].node->GetChildCount();  // only get the root nodes child

		for (int k = 0; k < childCount; ++k)
		{
			FbxNode* childNode = fbxJoints[i].node->GetChild(k);
			//childNode->GetSkeleton()->GetNodeCount()
			FbxJoint temp;
			if (childNode->GetSkeleton())
			{

				temp.node = childNode; //childNode;?
				temp.parentIndex = i;

				fbxJoints.push_back(temp);
			}

		}
	}


	for (int h = 0; h < fbxJoints.size(); ++h)
	{
		FbxMatrix mtx = fbxJoints[h].node->EvaluateGlobalTransform();
		Joint jointTrans;

		auto dxMtx = FbxMtx4toDxMtx(&mtx);
		DirectX::XMMatrixDecompose(&jointTrans.trans.scale, &jointTrans.trans.rotation, &jointTrans.trans.pos, dxMtx);
		jointTrans.parentIndex = fbxJoints[h].parentIndex;
		mesh->joints.push_back(jointTrans);

	}


	AnimationClip& animClip = mesh->animclip;

	FbxAnimStack* animStack = scene->GetCurrentAnimationStack();
	FbxTimeSpan timeSpan = animStack->GetLocalTimeSpan();
	FbxTime fbxtime = timeSpan.GetDuration();
	FbxLongLong frameCount = fbxtime.GetFrameCount(FbxTime::EMode::eFrames24);
	animClip.duration = fbxtime.GetSecondDouble();

	for (auto i = 0; i < frameCount; ++i)
	{
		KeyFrame keyFrame;
		fbxtime.SetFrame(i, FbxTime::EMode::eFrames24); //gives 24 frames per second;
		keyFrame.time = fbxtime.GetSecondDouble();

		for (int n = 0; n < fbxJoints.size(); ++n)
		{
			FbxMatrix mtx = fbxJoints[n].node->EvaluateGlobalTransform(fbxtime);
			auto dxMtx = FbxMtx4toDxMtx(&mtx);

			Transform t;

			DirectX::XMMatrixDecompose(&t.scale, &t.rotation, &t.pos, dxMtx);
			keyFrame.joints.push_back(t);
		}

		animClip.frames.push_back(keyFrame);
	}
	//mesh->animclip.push_back(animClip);
	//mesh->animClip[i].frames.push_back(keyFrame);

	FbxNode* poseNode = pose->GetNode(0);
	FbxMesh* tempMesh = poseNode->GetMesh();

	int controlPtInMesh = tempMesh->GetControlPointsCount();
	controlPtInfluence.resize(controlPtInMesh);

	if (tempMesh == nullptr)
		return; // See documentation for specifics.!!!!
	int deformerCount = tempMesh->GetDeformerCount();
	for (int j = 0; j < deformerCount; ++j)
	{
		FbxDeformer* tempDeformer = tempMesh->GetDeformer(j);
		int clusterCount;
		if (tempDeformer->Is<FbxSkin>())
		{
			clusterCount = ((FbxSkin*)tempDeformer)->GetClusterCount();
		}
		for (int i = 0; i < clusterCount; ++i)
		{
			FbxCluster* fbxCluster = ((FbxSkin*)tempDeformer)->GetCluster(i);
			fbxClusters.push_back(fbxCluster);
		}
	}

	int jointIndex;

	for (int k = 0; k < fbxClusters.size(); ++k)
	{
		FbxNode* linkNode = fbxClusters[k]->GetLink();
		for (int i = 0; i < fbxJoints.size(); ++i)
		{
			if (fbxJoints[i].node == linkNode)
			{
				jointIndex = i;
				break;
			}
		}

		int cpIndicesCount = fbxClusters[k]->GetControlPointIndicesCount();
		int* cpIndices = fbxClusters[k]->GetControlPointIndices();
		double* cpWeights = fbxClusters[k]->GetControlPointWeights();

		for (int i = 0; i < cpIndicesCount; ++i)
		{
			Influence tempInfluence;

			tempInfluence.joint = jointIndex;
			tempInfluence.weight = cpWeights[i];
			int minIndex = 0;
			for (int j = 0; j < 4; ++j)
			{
				if (controlPtInfluence[cpIndices[i]][j].weight < controlPtInfluence[cpIndices[i]][minIndex].weight)
				{
					minIndex = j;
				}

				//		
				if (tempInfluence.weight > controlPtInfluence[cpIndices[i]][minIndex].weight)
				{
					controlPtInfluence[cpIndices[i]][minIndex] = tempInfluence;
				}

			}



		}

	}
	for (int i = 0; i < controlPtInfluence.size(); ++i)
	{
		float sum = controlPtInfluence[i][0].weight + controlPtInfluence[i][1].weight + controlPtInfluence[i][2].weight + controlPtInfluence[i][3].weight;
		controlPtInfluence[i][0].weight /= sum;
		controlPtInfluence[i][1].weight /= sum;
		controlPtInfluence[i][2].weight /= sum;
		controlPtInfluence[i][3].weight /= sum;
	}

	ProcessFbxSkinnedMesh(tempMesh, controlPtInfluence, model);
}

string GetFileNameWithoutExtension(string input)
{
	int start = input.find_last_of("\\");
	int end = input.find_last_of(".");

	int len = end - start - 1;

	return input.substr(start + 1, len);
}

bool ImportFbxModel(const char * FileName, Mesh& model, JointData& jointdata)
{
	vector<influenceSet> tempSet;

	const char* lFilename = FileName;// "Axe Asset\\Axe_1.fbx";

	FbxManager* lSdkManager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings()))
	{
		return false;
	}

	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
	lImporter->Import(lScene);
	lImporter->Destroy();
	//ProcessFbxMesh(lScene->GetRootNode(), &model);
	GetJointsAnimationData(lScene->GetRootNode(), lScene, &jointdata, &model);

	// Load Material
	int num_mats = lScene->GetMaterialCount();
	for (int m = 0; m < num_mats; ++m)
	{
		FbxSurfaceMaterial* mat = lScene->GetMaterial(m);
		if (mat->Is<FbxSurfaceLambert>() == false) // Non-standard material, skip for now
			continue;
		FbxSurfaceLambert* lam = (FbxSurfaceLambert*)mat;
		if (FbxFileTexture* file_texture = lam->Diffuse.GetSrcObject<FbxFileTexture>())
		{
			string file_name = file_texture->GetRelativeFileName();

			file_name = GetFileNameWithoutExtension(file_name) + ".dds";

			file_path_t file_path;

			strcpy_s(file_path.data(), file_path.size() * sizeof(char),file_name.c_str());
			model.textures.push_back(file_path);
		}
		// Get emissive property as above
		if (FbxFileTexture* file_texture = lam->Emissive.GetSrcObject<FbxFileTexture>())
		{
			string file_name = file_texture->GetRelativeFileName();

			file_name = GetFileNameWithoutExtension(file_name) + ".dds";

			file_path_t file_path;

			strcpy_s(file_path.data(), file_path.size() * sizeof(char),file_name.c_str());
			model.textures.push_back(file_path);
		}
		// ...
		if (mat->Is<FbxSurfacePhong>())
		{
			FbxSurfacePhong* ph = (FbxSurfacePhong*)mat;

			// Get specular related properties...
			if (FbxFileTexture* file_texture = ph->Specular.GetSrcObject<FbxFileTexture>())
			{
				string file_name = file_texture->GetRelativeFileName();

				file_name = GetFileNameWithoutExtension(file_name) + ".dds";

				file_path_t file_path;

				strcpy_s(file_path.data(),file_path.size()* sizeof(char),file_name.c_str());
				model.textures.push_back(file_path);
			}
		}
		// add my_mat materials vector...


	}


	return true;
}


