#pragma once
#include"SparkRabbit/Render/GraphicsContext.h"

struct GLFWwindow;

namespace SparkRabbit {
	class OpenglContext : public GraphicsContext
	{
	public:
		OpenglContext(GLFWwindow* windowHandle);
		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_windowHandle;
	};
}

