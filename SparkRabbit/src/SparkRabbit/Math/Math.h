#pragma once
namespace SparkRabbit::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	glm::mat3 CalculateCovMatrix(std::vector<glm::vec3>& vertices, glm::vec3& outAvgPos);


	void JacobiSolver(glm::mat3 matrix, glm::vec3& eValues, glm::mat3& eVectors);


	void SchmidtOrthogonalization(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2);
}