#pragma once
#include<glm.hpp>
#include"gtc/matrix_transform.hpp"

namespace SparkRabbit {
	void computeMatricesFromInputs();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
}


