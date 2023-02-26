#pragma once


#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();