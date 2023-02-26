#pragma once
#include"event.h"
#include"../Core.h"


namespace SparkRabbit {

	class SR_API WindowCloseEvent : public Event
	{
	public:

		EVENT_TYPE(WindowClose)
		virtual std::string ToString() const override
		{
			std::stringstream i;
			i << "Event: Window Close";
			return i.str();
		}

	};


	class SR_API WindowResizeEvent : public Event
	{
	public:

		WindowResizeEvent(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}

		EVENT_TYPE(WindowResize)
		virtual std::string ToString() const override
		{
			std::stringstream i;
			i << "Event: Window size is " << m_width << " * " << m_height;
			return i.str();
		}

		inline unsigned int getWidth() { return m_width; }
		inline unsigned int getHeight() { return m_height; }

	private:
		unsigned int m_width, m_height;
	};

}
