#pragma once
#include <vector>
#include <glm.hpp>
#include "Load.h"
#include "Shader.h"
#include "Texture.h"
#include "Version.h"
#include "VboHandler.h"

namespace SparkRabbit {
	void Render(const char* filePath, const char* texturePath, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals);
}


