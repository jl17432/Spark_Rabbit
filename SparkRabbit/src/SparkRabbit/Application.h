#pragma once
#include"Core.h"
#include"EventsSystem/event.h"
#include"Window.h"
#include"Layer/LayerStack.h"
#include"SparkRabbit/EventsSystem/WindowEvent.h"
#include"SparkRabbit/EventsSystem/MouseEvent.h"

namespace SparkRabbit {
	class SR_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void PushEndLayer(Layer* endlayer);
		void PopEndLayer(Layer* endlayer);
	
		bool OnWindowClose(WindowCloseEvent& event);
		bool OnMouseMove(MouseMoveEvent& event);
		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		std::unique_ptr<Window>m_Window;
		bool m_running = true;
		LayerStack m_Layerstack;
	private:
		static Application* s_Instance;

	};

	Application* CreateApplication();
}


