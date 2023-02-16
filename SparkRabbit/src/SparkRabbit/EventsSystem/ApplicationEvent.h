#pragma once
#include"event.h"
#include"../Core.h"


namespace SparkRabbit {

	class SR_API AppTickEvent : public Event
	{
	public:

		EVENT_TYPE(AppTick)
		virtual std::string ToString() const override
		{
			return "Event: App Tick";
		}

	};


	class SR_API AppUpdateEvent : public Event
	{
	public:

		EVENT_TYPE(AppUpdate)
		virtual std::string ToString() const override
		{
			return "Event: App Update";
		}

	};

	class SR_API AppRenderEvent : public Event
	{
	public:
		EVENT_TYPE(AppRender)
		virtual std::string ToString() const override
		{
			return "Event: App Render";
		}

	};

}
