#pragma once
#include"event.h"
#include"../Core.h"


namespace SparkRabbit {
	class SR_API KeyPressEvent : public Event
	{
	public:
		KeyPressEvent(unsigned int key, bool is_Repeat) : m_key(key), m_isrepeat(is_Repeat) {}

		EVENT_TYPE(KeyPress)
		virtual std::string ToString()  const override
		{
			std::stringstream i;
			i << "Event: Key "<<m_key<<" pressed ";
			return i.str();
		}

		inline unsigned int getKey() { return m_key; }
		bool isrepeat() { return m_isrepeat; }

	private:
		unsigned int m_key;
		bool m_isrepeat;

	};


	class SR_API KeyReleaseEvent : public Event
	{
	public:
		KeyReleaseEvent(unsigned int key) : m_key(key) {}

		EVENT_TYPE(KeyRealease)
		virtual std::string ToString()  const override
		{
			std::stringstream i;
			i << "Event: Key " << m_key << " released ";
			return i.str();
		}

		inline unsigned int getKey() { return m_key; }

	private:
		unsigned int m_key;

	};

	class SR_API KeyTypeEvent : public Event
	{
	public:
		KeyTypeEvent(unsigned int key) : m_key(key) {}

		EVENT_TYPE(KeyType)
			virtual std::string ToString()  const override
		{
			std::stringstream i;
			i << "Event: Key " << m_key << " typed ";
			return i.str();
		}

		inline unsigned int getKey() { return m_key; }

	private:
		unsigned int m_key;

	};
}