#pragma once

#include "SparkRabbit/Layer.h"
#include "SparkRabbit/Events/MouseEvent.h"
#include "SparkRabbit/Events/KeyEvent.h"
#include "SparkRabbit/Events/ApplicationEvent.h"

namespace SparkRabbit {
	
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(const std::string& name);
		virtual ~ImGuiLayer();

		void Begin();
		void End();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
	private:
		float m_Time = 0.0f;
	};
}
