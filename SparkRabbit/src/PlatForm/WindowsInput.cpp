#include "PreCompile.h"
#include "WindowsInput.h"

#include"SparkRabbit/Application.h"
#include<GLFW/glfw3.h>

namespace SparkRabbit {
	Input* Input::s_instance = new WindowsInput();

	bool WindowsInput::isKeyPressImpl(int key)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, key);
		if (state == GLFW_PRESS || state == GLFW_REPEAT)
		{
			return true;
		}
		return false;
	}
	bool WindowsInput::isMousePressImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		if (state == GLFW_PRESS)
		{
			return true;
		}
		return false;
	}
	float WindowsInput::getMouseXImpl()
	{
		auto [x,y] = getMousePosImpl();
		return x;
	}
	float WindowsInput::getMouseYImpl()
	{
		auto [x, y] = getMousePosImpl();
		return y;
	}
	std::pair<float, float> WindowsInput::getMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos,(float)ypos };
	}
}

