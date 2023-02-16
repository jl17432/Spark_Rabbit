#include"PreCompile.h"
#include"WindowsWindow.h"
#include"SparkRabbit/EventsSystem/MouseEvent.h"
#include"SparkRabbit/EventsSystem/KeyboardEvent.h"
#include"SparkRabbit/EventsSystem/WindowEvent.h"
#include"SparkRabbit/EventsSystem/ApplicationEvent.h"
#include<glad/glad.h>

namespace SparkRabbit {
	Window* Window::Create(const WindowProperties& winprops)
	{
		return new WindowsWindow(winprops);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& winprops) : m_windowInfo(winprops.WindowName,winprops.width, winprops.height)
	{
		Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		glfwDestroyWindow(m_Window);
	}

	bool SR_GLFWInitialized = false;

	void WindowsWindow::Init()
	{
		if (!SR_GLFWInitialized) {
			int success = glfwInit();
			SR_CORE_ASSERT(!success, "InitGLFW failed");
			SR_GLFWInitialized = true;
		}
		m_Window = glfwCreateWindow(m_windowInfo.width, m_windowInfo.height, m_windowInfo.WindowName.c_str(),nullptr,nullptr);
		glfwMakeContextCurrent(m_Window);
		int flag = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SR_CORE_ASSERT(flag, "Fail to load GLAD");
		glfwSetWindowUserPointer(m_Window, &m_windowInfo);
		SetSync(true);

		//glfw回调函数让事件显示到console上输出
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				auto info = (WindowInfo*)glfwGetWindowUserPointer(window);
				info->width = width;
				info->height = height;

				WindowResizeEvent windowResizeEvent(info->width, info->height);
				info->ecall(windowResizeEvent);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				auto info = (WindowInfo*)glfwGetWindowUserPointer(window);
				WindowCloseEvent windowCloseEvent;
				info->ecall(windowCloseEvent);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				auto info = (WindowInfo*)glfwGetWindowUserPointer(window);
				MouseMoveEvent mouseMoveEvent(xpos, ypos);
				info->ecall(mouseMoveEvent);
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				auto info = (WindowInfo*)glfwGetWindowUserPointer(window);
				MouseScrollEvent mouseScrolledEvent(xoffset, yoffset);
				info->ecall(mouseScrolledEvent);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				auto info = (WindowInfo*)glfwGetWindowUserPointer(window);
				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseClickEvent mouseClickEvent(button, action);
					info->ecall(mouseClickEvent);
				}break;
				case GLFW_RELEASE:
				{
					MouseReleaseEvent mouseReleaseEvent(button);
					info->ecall(mouseReleaseEvent);
				}break;
				default:
					break;
				}
			});


		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				auto info = (WindowInfo*)glfwGetWindowUserPointer(window);
				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressEvent keyPressEvent(key, action);
					info->ecall(keyPressEvent);
				}break;
				case GLFW_RELEASE:
				{
					KeyReleaseEvent keyReleaseEvent(key);
					info->ecall(keyReleaseEvent);
				}break;
				default:
					break;
				}
			});


	}

	void WindowsWindow::Update() 
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetSync(bool value)
	{
		if (value)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
		bVSync = value;
	}
}