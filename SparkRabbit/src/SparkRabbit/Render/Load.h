#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace SparkRabbit {
	bool LoadObjModel(const char* filePath, std::vector<glm::vec3>& Obj_Vertices, std::vector<glm::vec2>& Obj_Texture, std::vector<glm::vec3>& Obj_Normals);
}