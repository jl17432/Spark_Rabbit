#pragma once
#include"Core.h"
#include "Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Window.h"
#include "Hazel/LayerStack.h"
namespace Hazel
{
	class _declspec(dllexport) Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};
	//To be defined in Client
	Application* CreateApplication();

}

