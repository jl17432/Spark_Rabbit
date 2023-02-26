#pragma once

#include "SparkRabbit/Core.h"
#include "Layer.h"

#include <vector>

typedef std::vector<SparkRabbit::Layer*> HazelLayer;
namespace SparkRabbit 
{
	class SPARK_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		HazelLayer::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}

