#pragma once
#include"SparkRabbit/Layer/Layer.h"

namespace SparkRabbit {
	class SR_API ImguiLayer : public Layer
	{
	public:

		ImguiLayer();
		~ImguiLayer();

		// ͨ�� Layer �̳�
		void OnAttach();
		void OnDetach();
		void update();
		void onEvent(Event& event);


	private:
		float m_time = 0.0f;
	};
}

