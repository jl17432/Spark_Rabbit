#pragma once
#include"SparkRabbit/Layer/Layer.h"
#include"SparkRabbit/EventsSystem/ApplicationEvent.h"
#include"SparkRabbit/EventsSystem/KeyboardEvent.h"
#include"SparkRabbit/EventsSystem/MouseEvent.h"
#include"SparkRabbit/EventsSystem/WindowEvent.h"

namespace SparkRabbit {
	class SR_API ImguiLayer : public Layer
	{
	public:

		ImguiLayer();
		~ImguiLayer();

		// Í¨¹ý Layer ¼Ì³Ð
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void onImguiRender() override;

		void begin();
		void end();


	private:
		float m_time = 0.0f;
	};
}

