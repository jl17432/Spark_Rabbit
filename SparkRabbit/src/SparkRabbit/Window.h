#pragma once
#include"PreCompile.h"
#include"EventsSystem/event.h"
#include"Core.h"

namespace SparkRabbit {
	struct WindowProperties
	{
		std::string WindowName;
		unsigned int width;
		unsigned int height;

		WindowProperties(const std::string& m_WindowName = "SparkRabbit Engine", unsigned int m_width = 1280, unsigned int m_height = 720)
			: WindowName(m_WindowName), width(m_width), height(m_height){}
	};

	class SR_API Window
	{
	public:
		using EventFunc = std::function<void(Event&)>;
		virtual ~Window() {};
		virtual void Update() = 0;
		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;
		virtual void EventCallBack(const EventFunc& ecall) = 0;
		virtual void SetSync(bool value) = 0;
		static Window* Create(const WindowProperties& winprops = WindowProperties());
		virtual void* GetNativeWindow() const = 0;
	};
}