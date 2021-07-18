
#include "ModelFileImporter.h"
#include<fstream>
using namespace std;
using namespace FVertex;

bool FileImport::Import::ReadBinFile(const char* filename, MeshImport* mesh)
{
	assert(mesh != nullptr);
	ifstream reader;

	reader.open(filename, std::ios_base::binary);
	if (reader.is_open())
	{
		uint32_t vertexCount;
		uint32_t indexCount;
		uint8_t textureCount;

		reader.read((char*)&vertexCount, sizeof(vertexCount));
		reader.read((char*)&indexCount, sizeof(indexCount));
		reader.read((char*)&textureCount, sizeof(textureCount));

		mesh->vertices.resize(vertexCount);
		mesh->indices.resize(indexCount);
		mesh->textures.resize(textureCount);

		reader.read((char*)mesh->vertices.data(), sizeof(Vertex) * vertexCount);
		reader.read((char*)mesh->indices.data(), sizeof(uint32_t) * indexCount);

		for (int i = 0; i < textureCount; ++i)
		{
			reader.read((char*)mesh->textures[i].data(), file_path_size);
		}
		reader.close();

		return true;
	}
	return false;
}
