#pragma once

#include "SparkRabbit/Layer.h"
#include "SparkRabbit/Events/MouseEvent.h"
#include "SparkRabbit/Events/KeyEvent.h"
#include "SparkRabbit/Events/ApplicationEvent.h"

namespace SparkRabbit {
	
	class SPARK_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();

		void OnUpdate();
		void OnEvent(Event& event);

	private:

		bool OnMouseButtonPreassedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);


		float m_Time = 0.0f;
	};
}
