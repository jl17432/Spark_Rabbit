#include "PreCompile.h"
#include "OpenglContext.h"
#include"GLFW/glfw3.h"
#include<glad/glad.h>
#include"SparkRabbit/Core.h"
#include<gl/GL.h>


namespace SparkRabbit {
	OpenglContext::OpenglContext(GLFWwindow* windowHandle) : m_windowHandle(windowHandle)
	{
		SR_CORE_ASSERT(windowHandle, "Window Handle is null");
	}
	void OpenglContext::Init()
	{
		glfwMakeContextCurrent(m_windowHandle);
		int flag = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SR_CORE_ASSERT(flag, "Fail to load GLAD");
	}
	void OpenglContext::SwapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}
}
