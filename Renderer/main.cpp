#include <filesystem>
namespace fs = std::filesystem;

#include "utils/Render.h"

int main()
{
	//std::string filePath = "objects";
	//for (const auto& objects : fs::directory_iterator(filePath)) {
	//	std::cout << objects << std::endl;
	//}

	// ´´½¨ vector
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	const char* filePath = "cylinder.obj";
	const char* texturePath = "uvmap.dds";

	Render(filePath, texturePath, vertices, uvs, normals, tangents, bitangents);
	return 0;
}