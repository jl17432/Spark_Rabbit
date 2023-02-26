#pragma once
#include"Core.h"
#include"EventsSystem/event.h"
#include"Window.h"
#include"Layer/LayerStack.h"
#include"SparkRabbit/EventsSystem/WindowEvent.h"
#include"SparkRabbit/EventsSystem/MouseEvent.h"
#include"Imgui/ImguiLayer.h"

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
	
		void Close();
		bool OnWindowClose(WindowCloseEvent& event);
		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		std::unique_ptr<Window>m_Window;
		ImguiLayer* m_ImguiLayer;
		bool m_running = true;
		LayerStack m_Layerstack;
	private:
		static Application* s_Instance;

	};

	Application* CreateApplication();
}


