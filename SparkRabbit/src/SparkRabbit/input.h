#pragma once
#include"Core.h"

namespace SparkRabbit {
	class SR_API Input
	{
	public:

		inline static bool isKeyPress(int key) { return s_instance->isKeyPressImpl(key); }
		inline static bool isMousePress(int button) { return s_instance->isMousePressImpl(button); }
		inline static float getMouseX() { return s_instance->getMouseX(); }
		inline static float getMouseY() { return s_instance->getMouseY(); }
		inline static std::pair<float, float> getMousePos() { return s_instance->getMousePosImpl(); }
	protected:
		virtual bool isKeyPressImpl(int key) = 0;
		virtual bool isMousePressImpl(int button) = 0;
		virtual float getMouseXImpl() = 0;
		virtual float getMouseYImpl() = 0;
		virtual std::pair<float,float>getMousePosImpl() = 0;
	private:
		static Input* s_instance;

	};
}
