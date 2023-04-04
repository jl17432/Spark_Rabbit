#pragma once

namespace SparkRabbit {
	class TickTime
	{
	public:
		TickTime() = default;
		TickTime(float time) : m_Time(time) {}

		operator float() const { return m_Time; }
		inline float GetTime_s() const { return m_Time; }
		inline float GetTime_ms() const { return m_Time * 1000.0f; }
	private:
		float m_Time = 0.0f;
	};
}