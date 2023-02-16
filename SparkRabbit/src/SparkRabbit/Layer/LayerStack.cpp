#include "PreCompile.h"
#include "LayerStack.h"
#include"SparkRabbit/Log.h"

namespace SparkRabbit {
	LayerStack::LayerStack()
	{
		m_layerIterator = m_layer.begin();
	}


	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_layer)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_layerIterator = m_layer.emplace(m_layerIterator, layer);
	}

	void LayerStack::PopLayer(Layer* layer) {
		auto it = std::find(m_layer.begin(), m_layer.end(), layer);
		if (it != m_layer.end())
		{
			m_layer.erase(it);
			m_layerIterator--;

		}
		else
		{
			SR_CORE_WARN("This layer is not in the stack");
		}
	}

	void LayerStack::PushEndLayer(Layer* endlayer)
	{
		m_layer.emplace_back(endlayer);
	}

	void LayerStack::PopEndLayer(Layer* endlayer)
	{
		auto it = std::find(m_layer.begin(), m_layer.end(), endlayer);
		if (it != m_layer.end())
		{
			m_layer.erase(it);

		}
		else
		{
			SR_CORE_WARN("This layer is not in the stack");
		}
	}

}
