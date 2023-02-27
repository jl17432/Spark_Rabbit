#pragma once
#include"SparkRabbit/Window.h"
#include"SparkRabbit/Render/GraphicsContext.h"
#include<GLFW/glfw3.h>

namespace SparkRabbit {
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& winprops);
		virtual ~WindowsWindow();

		void Update() override;
		inline virtual unsigned int GetWidth() override { return m_windowInfo.width; };
		inline virtual unsigned int GetHeight() override { return m_windowInfo.height; };
		virtual void EventCallBack(const EventFunc& ecall) override { m_windowInfo.ecall = ecall; }
		virtual void SetSync(bool value) override;
		void Init();
		inline virtual void* GetNativeWindow() const override { return m_Window; }

	private:
		struct WindowInfo
		{
			std::string WindowName;
			unsigned int width;
			unsigned int height;
			EventFunc ecall;
			WindowInfo(std::string m_WindowName, unsigned int m_width, unsigned int m_height) : WindowName(m_WindowName), width(m_width), height(m_height){}
		};
		GLFWwindow* m_Window;
		GraphicsContext* m_context;
		WindowInfo m_windowInfo;
		bool bVSync;


	};
}