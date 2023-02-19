#pragma once

#include "SparkRabbit/Core.h"
#include "SparkRabbit/Events/Event.h"


namespace SparkRabbit
{
	class SPARK_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}
