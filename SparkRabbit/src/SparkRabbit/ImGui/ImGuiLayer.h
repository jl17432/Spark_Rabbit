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

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
		
	private:
		float m_Time = 0.0f;
	};
}
