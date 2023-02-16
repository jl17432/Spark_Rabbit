#pragma once
#include"SparkRabbit/Core.h"
#include"SparkRabbit/EventsSystem/event.h"

namespace SparkRabbit {
	class SR_API Layer
	{
	public:
		Layer(std::string name = "Layer");
		virtual ~Layer() = default;
		virtual void OnAttach(){};
		virtual void OnDetach(){};
		virtual void update(){};
		virtual void onEvent(Event& event){};
	protected:
		std::string m_layername;
	};
}

