#pragma once
#include "SparkRabbit/Renderer/RenderContext.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<GL/GL.h>

namespace SparkRabbit {

	class OpenGLContext : public RenderContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}