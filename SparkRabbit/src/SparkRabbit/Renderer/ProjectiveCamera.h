#pragma once

#include "glm/glm.hpp"
#include "SparkRabbit/TickTime.h"
#include "SparkRabbit/Events/MouseEvent.h"

namespace SparkRabbit{

	class ProjectiveCamera
	{
	public:
		ProjectiveCamera() = default;
		ProjectiveCamera(const glm::mat4& projectionMatrix);


		void OnUpdate(TickTime time);
		void OnEvent(Event& event);

		glm::vec3 GetCameraUp();
		glm::vec3 GetCameraRight();
		glm::vec3 GetCameraFront();

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
		inline void SetProjection(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }

		inline const glm::mat4& GetView() const { return m_ViewMatrix; }
		inline const glm::mat4& GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }
		inline const glm::vec3& GetPosition() const { return m_Position; }

		inline void SetViewport(uint32_t width, uint32_t height) { m_Width = width; m_Height = height; }

		inline float& GetExposure() { return m_Exposure; }
	
	private:
		void UpdateCameraView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition();
		glm::quat GetOrientation();

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

	private:
		float m_Distance;
		float m_Pitch;
		float m_Yaw;
		float m_Exposure = 1.0f;

		uint32_t m_Width;
		uint32_t m_Height;

		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };
		
		bool m_Panning, m_Rotating;
		glm::vec2 m_PreviousMousePosition;
		glm::vec3 m_InitialFocalPoint, m_InitialRotation;

	};
}
