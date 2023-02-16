#pragma once
#include"Core.h"
#include"spdlog/spdlog.h"
#include"spdlog/fmt/ostr.h"

namespace SparkRabbit {
	class SR_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

//Core log macros
#define SR_CORE_ERROR(...)  ::SparkRabbit::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SR_CORE_WARN(...)   ::SparkRabbit::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SR_CORE_INFO(...)   ::SparkRabbit::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SR_CORE_TRACE(...)  ::SparkRabbit::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SR_CORE_FATAL(...)  ::SparkRabbit::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Client log macros
#define SR_ERROR(...)       ::SparkRabbit::Log::GetClientLogger()->error(__VA_ARGS__)
#define SR_WARN(...)        ::SparkRabbit::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SR_INFO(...)        ::SparkRabbit::Log::GetClientLogger()->info(__VA_ARGS__)
#define SR_TRACE(...)       ::SparkRabbit::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SR__FATAL(...)      ::SparkRabbit::Log::GetClientLogger()->fatal(__VA_ARGS__)

