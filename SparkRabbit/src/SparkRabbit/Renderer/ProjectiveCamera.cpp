#include "PrecompileH.h"
#include "ProjectiveCamera.h"

#include "SparkRabbit/Input.h"

#include "SparkRabbit/InputCodes.h"

#include "glm/gtc/quaternion.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define M_PI 3.14159f

namespace SparkRabbit {
	ProjectiveCamera::ProjectiveCamera(const glm::mat4& projectionMatrix)
		: m_ProjectionMatrix(projectionMatrix)
	{
		m_Rotation = glm::vec3(90.0f, 0.0f, 0.0f);
		m_FocalPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		//m_Distance = glm::distance(m_Position, m_FocalPoint);
		m_Distance = glm::distance(glm::vec3(-5.f,5.f,5.f), m_FocalPoint);

		m_Yaw = 3.0f * (float)M_PI / 4.0f;
		m_Pitch = M_PI / 4.0f;

		UpdateCameraView();
	}
	void ProjectiveCamera::OnUpdate(TickTime time)
	{
		if (Input::IsKeyPressed(SR_KEY_LEFT_ALT))
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY()};
			glm::vec2 delta = mouse - m_PreviousMousePosition;
			m_PreviousMousePosition = mouse;

			delta *= time.GetTime_s();

			if(Input::IsMouseButtonPressed(SR_MOUSE_BUTTON_LEFT))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(SR_MOUSE_BUTTON_MIDDLE))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(SR_MOUSE_BUTTON_RIGHT))
				MouseZoom((float)delta.y);
		}
		else
		{
			m_PreviousMousePosition = { Input::GetMouseX(), Input::GetMouseY()};
		}

		UpdateCameraView();

	}
	void ProjectiveCamera::OnEvent(Event& event)	
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseScrolledEvent>(SPARK_BIND_EVENT_FN(ProjectiveCamera::OnMouseScroll));
	}
	glm::vec3 ProjectiveCamera::GetCameraUp()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	glm::vec3 ProjectiveCamera::GetCameraRight()
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	glm::vec3 ProjectiveCamera::GetCameraFront()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}
	void ProjectiveCamera::UpdateCameraView()
	{
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_Rotation = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}
	bool ProjectiveCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		return false;
	}
	void ProjectiveCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetCameraRight() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetCameraUp() * delta.y * ySpeed * m_Distance;
	}
	void ProjectiveCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetCameraUp().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
		//m_Pitch = std::max(m_Pitch, 0.001f);
		//m_Pitch = std::min(m_Pitch, (float)M_PI - 0.001f);
	}
	void ProjectiveCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetCameraFront();
			m_Distance = 1.0f;
		}
	}
	glm::vec3 ProjectiveCamera::CalculatePosition()
	{
		return m_FocalPoint - GetCameraFront() * m_Distance;
	}
	glm::quat ProjectiveCamera::GetOrientation()
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}
	std::pair<float, float> ProjectiveCamera::PanSpeed() const
	{
		float x = std::min(m_Width / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_Height / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}
	float ProjectiveCamera::RotationSpeed() const
	{
		return 0.8f;
	}
	float ProjectiveCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f);
		return speed;
	}
}
