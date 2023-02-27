#pragma once
#include"PreCompile.h"
#include"../Core.h"

namespace SparkRabbit {
	//ö���¼�
	enum class Type {
		null = 0,
		WindowClose,
		WindowResize,
		KeyPress,
		KeyRealease,
		KeyType,
		MouseClick,
		MouseRealease,
		MouseMove,
		MouseScroll,
		AppTick,
		AppUpdate,
		AppRender
	};

#define EVENT_TYPE(type)static Type getStaticEventType(){return Type::type;}\
						virtual Type getEventType()const override { return getStaticEventType(); }\
						virtual const std::string getInfo() const override { return #type; }

	class Event {
	public:
		friend class EventDispatcher;
		virtual ~Event() {};
		virtual Type getEventType() const = 0;
		virtual const std::string getInfo()const = 0;
		virtual std::string ToString()const{ return getInfo(); }
		bool GetStatus() const { return handled; }

	private:
		bool handled = false;
	};
	//�����¼�����
	class EventDispatcher
	{
		template<typename T>
		using EventDispatchFunc = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event) : m_Event(event){}//�洢һ���¼�

		template<typename T>
		void Dispatch(EventDispatchFunc<T> func)
		{
			if (m_Event.getEventType() == T::getStaticEventType())
			{
				m_Event.handled = func(*(T*)&m_Event);
			}
		}
	private:
		Event& m_Event;
	};
	//����<<��������ֹfmt����
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}


