#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
namespace SparkRabbit {
	class SPARK_API Log
	{
	public:
		static void Init();

		inline static auto& GetCoreLogger() { return s_CoreLogger; }
		inline static auto& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//Logging macros
#define SPARK_CORE_TRACE(...)			::SparkRabbit::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SPARK_CORE_INFO(...)			::SparkRabbit::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SPARK_CORE_WARN(...)			::SparkRabbit::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SPARK_CORE_ERROR(...)			::SparkRabbit::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SPARK_CORE_CRITICAL(...)		::SparkRabbit::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define SPARK_TRACE(...)				::SparkRabbit::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SPARK_INFO(...)					::SparkRabbit::Log::GetClientLogger()->info(__VA_ARGS__)
#define SPARK_WARN(...)					::SparkRabbit::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SPARK_ERROR(...)				::SparkRabbit::Log::GetClientLogger()->error(__VA_ARGS__)
#define SPARK_CRITICAL(...)				::SparkRabbit::Log::GetClientLogger()->critical(__VA_ARGS__)
