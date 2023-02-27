#include "PreCompile.h"
#include "Load.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

namespace SparkRabbit {

	bool LoadObjModel(const char* filePath, std::vector<glm::vec3>& Obj_Vertices, std::vector<glm::vec2>& Obj_Texture, std::vector<glm::vec3>& Obj_Normals)

	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene) //��ⱨ��
		{
			std::cout << import.GetErrorString() << std::endl;
			return false;
		}

		const int numOfMesh = scene->mNumMeshes;

		for (int mesh = 0; mesh < numOfMesh; mesh++) //������mesh
		{
			auto currMesh = scene->mMeshes[mesh]; //����mesh

			// mesh�ᶥ��
			Obj_Vertices.reserve(currMesh->mNumVertices);
			for (unsigned int i = 0; i < currMesh->mNumVertices; i++) {
				auto pos = currMesh->mVertices[i];
				Obj_Vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
			}

			// mesh�����
			Obj_Texture.reserve(currMesh->mNumVertices);
			for (unsigned int i = 0; i < currMesh->mNumVertices; i++) {
				auto UVW = currMesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
				Obj_Texture.push_back(glm::vec2(UVW.x, UVW.y));
			}

			// mesh��normals
			Obj_Normals.reserve(currMesh->mNumVertices);
			for (unsigned int i = 0; i < currMesh->mNumVertices; i++) {
				auto n = currMesh->mNormals[i];
				Obj_Normals.push_back(glm::vec3(n.x, n.y, n.z));
			}
		}
		return true;
	}
}