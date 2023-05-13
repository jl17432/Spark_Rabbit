#include"PrecompileH.h"
#include"SceneCamera.h"

#include<glm/gtc/matrix_transform.hpp>

namespace SparkRabbit {

	// Set the camera projection type to perspective and configure the perspective settings
	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
	}

	// Set the camera projection type to orthographic and configure the orthographic settings
	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
	}

	// Set the viewport size and update the projection matrix accordingly
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
			// Update the perspective projection matrix based on the new viewport size
			m_ProjectionMatrix = glm::perspectiveFov(m_PerspectiveFOV, (float)width, (float)height, m_PerspectiveNear, m_PerspectiveFar);
			break;
		case ProjectionType::Orthographic:
			// Calculate the aspect ratio and update the orthographic projection matrix based on the new viewport size
			float aspect = (float)width / (float)height;
			float orthoWidth = m_OrthographicSize * aspect;
			float orthoHeight = m_OrthographicSize;
			m_ProjectionMatrix = glm::ortho(-orthoWidth * 0.5f, orthoWidth * 0.5f, -orthoHeight * 0.5f, orthoHeight * 0.5f);
			break;
		}
	}
}
