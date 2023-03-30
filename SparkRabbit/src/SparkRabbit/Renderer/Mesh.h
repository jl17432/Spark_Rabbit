#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "SparkRabbit/TickTime.h"
#include "SparkRabbit/Log.h"

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Assimp {
	class Importer;
}

namespace SparkRabbit {
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 Texcoord;
	};

	struct AnimatedVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 Texcoord;

		uint32_t IDs[4] = { 0,0,0,0 };
		float Weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		void AddBoneData(uint32_t boneID, float weight)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (Weights[i] = 0.0f)
				{
					IDs[i] = boneID;
					Weights[i] = weight;
					return;
				}
			}
			SPARK_CORE_WARN("Vertex has more than 4 bones,(BoneID={0}, Weight={1})", boneID, weight);
		}	
	};

	static const int NumAttributes = 5;

	struct Index
	{
		uint32_t V1, V2, V3;
	};

	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
	};

	struct VertexBoneData
	{
		uint32_t IDs[4] = { 0,0,0,0 };
		float Weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		VertexBoneData()
		{
			memset(IDs, 0, sizeof(IDs));
			memset(Weights, 0, sizeof(Weights));
		}

		void AddBoneData(uint32_t boneID, float weight)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (Weights[i] = 0.0f)
				{
					IDs[i] = boneID;
					Weights[i] = weight;
					return;
				}
			}
			SPARK_CORE_WARN("Vertex has more than 4 bones,(BoneID={0}, Weight={1})", boneID, weight);
		}
	};

	struct Triangle
	{
		Vertex V0, V1, V2;
		Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
			: V0(v0), V1(v1), V2(v2) {}
	};

	class SubMesh
	{
	public:
	public:
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;

		glm::mat4 Transform;
		//AABB BoundingBox;

		std::string NodeName, MeshName;
	};

	class Mesh
	{
	public:
		Mesh(const std::string& path);
		~Mesh();

		void OnUpdate(TickTime ts);
		//void LogVertexBuffer();

		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
		const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }

		//std::shared_ptr<Shader> GetMeshShader() { return m_MeshShader; }
		std::shared_ptr<Material>GetMaterial() { return m_Material; }



	private:
		std::vector<SubMesh> m_SubMeshes;
		std::vector<Vertex> m_Vertices;
		std::vector<AnimatedVertex> m_AnimatedVertices;
		std::vector<Index> m_Indices;
		std::unordered_map<std::string, uint32_t> m_BoneMapping;
		std::vector<glm::mat4> m_BoneTransforms;
		const aiScene* m_Scene;

		std::unique_ptr<Assimp::Importer> m_Importer;

		glm::mat4 m_InverseTransform;

		uint32_t m_BoneCount = 0;
	};

}
