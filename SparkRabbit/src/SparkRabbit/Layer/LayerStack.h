#pragma once
#include"SparkRabbit/Core.h"
#include"SparkRabbit/EventsSystem/event.h"
#include"Layer.h"

#include<vector>

namespace SparkRabbit {
	class SR_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushEndLayer(Layer* endlayer);
		void PopEndLayer(Layer* endlayer);

		std::vector<Layer*>::iterator begin() { return m_layer.begin(); }
		std::vector<Layer*>::iterator end() { return m_layer.end(); }

	private:
		std::vector<Layer*> m_layer;
		unsigned int  m_layerInsertIndex = 0;
	};
}
