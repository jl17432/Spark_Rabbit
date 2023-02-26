#pragma once

#ifdef SR_PLATFORM_WINDOWS
#if SR_DYNAMIC_LINK
	#ifdef SR_BUILD_DLL
		#define SPARK_API _declspec(dllexport)
	#else
		#define SPARK_API _declspec(dllimport)
	#endif
#else 
	#define SPARK_API
#endif
#else
	#error Hazel only support Windows!
#endif

#ifdef SPARK_DEBUG 
#define SPARK_ENABLE_ASSERTS
#endif

#ifdef SPARK_ENABLE_ASSERTS
	#define SPARK_ASSERT(x, ...) {if (!(x)) { SPARK_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define SPARK_CORE_ASSERT(x, ...) {if (!(x)) { SPARK_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define SPARK_ASSERT(x, ...)
	#define SPARK_CORE_ASSERT(X, ...)

#endif

#define BIT(x) (1 << x)

#define SPARK_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)