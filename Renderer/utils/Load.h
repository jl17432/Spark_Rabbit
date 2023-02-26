#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

bool LoadObjModel(const char* filePath, std::vector<glm::vec3>& Obj_Vertices, std::vector<glm::vec2>& Obj_Texture, std::vector<glm::vec3>& Obj_Normals);