#pragma once
#include"event.h"
#include"../Core.h"


namespace SparkRabbit {
	class SR_API KeyPressEvent : public Event
	{
	public:
		KeyPressEvent(unsigned int key, unsigned int count) : m_key(key), m_count(count){}

		EVENT_TYPE(KeyPress)
		virtual std::string ToString()  const override
		{
			std::stringstream i;
			i << "Event: Key "<<m_key<<" pressed "<<m_count<<" Times.";
			return i.str();
		}

		inline unsigned int getKey() { return m_key; }
		inline unsigned int getCount() { return m_count; }

	private:
		unsigned int m_key;
		unsigned int m_count;

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
}