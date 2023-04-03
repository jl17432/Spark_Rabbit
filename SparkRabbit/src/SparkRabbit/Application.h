#pragma once
#include"Core.h"
#include "Window.h"
#include "SparkRabbit/LayerStack.h"
#include "SparkRabbit/TickTime.h"

#include "SparkRabbit/Events/ApplicationEvent.h"

#include "SparkRabbit/ImGui/ImGuiLayer.h"


namespace SparkRabbit
{

	struct ApplicationProps
	{
		std::string Name;
		uint32_t WindowWidth, WindowHeight;
	};

	class SPARK_API Application
	{
	public:
		Application(const ApplicationProps& props = { "SparkRabbit Engine Demo", 1280, 720 });
		virtual ~Application();

		void Run();

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate(TickTime ts) {}

		virtual void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void RenderImGui();

		std::string OpenFile(const std::string& filter) const;

		inline Window& GetWindow() { return *m_Window; }

		static inline Application& Get() { return *s_Instance; }

		float GetTime() const; 
	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_Window;  //for now we only have one window
		bool m_Running = true, m_Minimized = false;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;
		TickTime m_TickTime;

		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};
	//To be defined in Client
	Application* CreateApplication();

}

