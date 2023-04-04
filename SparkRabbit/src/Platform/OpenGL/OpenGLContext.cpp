#include "PrecompileH.h"
#include "OpenGLContext.h"


namespace SparkRabbit {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		SPARK_CORE_ASSERT(windowHandle, "windowHandle is null");

	}
	
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SPARK_CORE_ASSERT(status, "Failed to initialize Glad");

		//SPARK_CORE_INFO("OpenGL Renderer:{0}", glGetString(GL_VENDOR));
		std::cout << "OpenGL Info:" << std::endl;
		std::cout << "  Vendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "  Renderer: " << glGetString(GL_RENDERER) <<std::endl;
		std::cout << "  Version: " << glGetString(GL_VERSION) << std::endl;
		
	}
	
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}