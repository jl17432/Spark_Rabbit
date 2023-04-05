#include"PrecompileH.h"
#include"Camera.h"

namespace SparkRabbit {
	Camera::Camera(const glm::mat4& projectionMatrix)
		: m_ProjectionMatrix(projectionMatrix)
	{
	}
}