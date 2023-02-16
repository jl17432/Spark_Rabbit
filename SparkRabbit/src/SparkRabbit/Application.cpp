#include"PreCompile.h"
#include "Application.h"
#include"EventsSystem/WindowEvent.h"
#include"Log.h"
#include<glad/glad.h>

namespace SparkRabbit {

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		SR_CORE_ASSERT(s_Instance, "Application is already exit");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		auto BindFunc = std::bind(&Application::OnEvent, this, std::placeholders::_1);
		m_Window->EventCallBack(BindFunc);

		unsigned int id;
		glGenVertexArrays(1, &id);
	}

	Application::~Application() {

	}

	void Application::PushLayer(Layer* layer)
	{
		m_Layerstack.PushLayer(layer);
		layer->OnAttach();

	}

	void Application::PopLayer(Layer* layer)
	{
		m_Layerstack.PopLayer(layer);

	}

	void Application::PushEndLayer(Layer* endlayer)
	{
		m_Layerstack.PushEndLayer(endlayer);
		endlayer->OnAttach();
	}

	void Application::PopEndLayer(Layer* endlayer)
	{
		m_Layerstack.PopEndLayer(endlayer);
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
		SR_CORE_TRACE("{0}", event);

		for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();)
		{
			(*--it)->onEvent(event);
			if (event.GetStatus())
			{
				break;
			}
		}
	}

	void Application::Run() {
		while (m_running)
		{
			glClearColor(0, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			for (Layer* layer : m_Layerstack)
			{
				layer->update();
			}
			m_Window->Update();
		}
	} 

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_running = false;
		return true;
	}
	
}
