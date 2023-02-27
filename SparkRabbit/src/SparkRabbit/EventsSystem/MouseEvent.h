#pragma once
#include"event.h"
#include"../Core.h"



namespace SparkRabbit {

	//Mouse click Event
	class SR_API MouseClickEvent : public Event
	{
	public:
		MouseClickEvent(unsigned int button) : m_button(button){}

		EVENT_TYPE(MouseClick)
		virtual std::string ToString() const override
		{
			std::stringstream i;
			i << "Event: Mouse " << m_button << " clicked ";
			return i.str();
		}

		inline unsigned int getButton() { return m_button; }
	private:
		unsigned int m_button;

	};

	////Mouse release Event
	class SR_API MouseReleaseEvent : public Event
	{
	public:
		MouseReleaseEvent(unsigned int button) : m_button(button) {}

		EVENT_TYPE(MouseRealease)
		virtual std::string ToString() const override
		{
			std::stringstream i;
			i << "Event: Mouse " << m_button << " released ";
			return i.str();
		}

		inline unsigned int getButton() { return m_button; }
	private:
		unsigned int m_button;
	};

	////Mouse move Event
	class SR_API MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(float posX, float posY) : m_posX(posX), m_posY(posY) {}

		EVENT_TYPE(MouseMove)
		virtual std::string ToString() const override
		{
			std::stringstream i;
			i << "Event: Mouse postion at (" << m_posX << " , " << m_posY << " )";
			return i.str();
		}

		inline float getPosX() { return m_posX; }
		inline float getPosY() { return m_posY; }
	private:
		float m_posX;
		float m_posY;
	};

	////Mouse move Event
	class SR_API MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(float xoffset, float yoffset) : m_xoffset(xoffset), m_yoffset(yoffset) {}

		EVENT_TYPE(MouseScroll)
		virtual std::string ToString() const override
		{
			std::stringstream i;
			i << "Event: Mouse scroll , postion at (" << m_xoffset << " , " << m_yoffset << " )";
			return i.str();
		}

		inline float getPosX() { return m_xoffset; }
		inline float getPosY() { return m_yoffset; }
	private:
		float m_xoffset;
		float m_yoffset;
	};
}