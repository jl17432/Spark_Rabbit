#pragma once
#include"SparkRabbit/input.h"

namespace SparkRabbit {
	class WindowsInput : public Input
	{
	protected:
		virtual bool isKeyPressImpl(int key) override;
		virtual bool isMousePressImpl(int button) override;
		virtual float getMouseXImpl() override;
		virtual float getMouseYImpl() override;
		virtual std::pair<float,float> getMousePosImpl() override;
	};
}

