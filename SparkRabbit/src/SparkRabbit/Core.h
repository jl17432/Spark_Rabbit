#pragma once
#ifdef SR_PLATFORM_WINDOWS
#if SR_DYNAMIC_LINK
    #ifdef SR_BUILD_DLL
        #define SR_API __declspec(dllexport)
    #else
        #define SR_API __declspec(dllimport)
    #endif // SR_BUILD_DLL
#else
    #define SR_API
#endif
#else
    #error SparkRabbit only support windows.
#endif // SR_PLATFORM_WINDOWS

#ifdef SR_ASSERT_ENABLE
    #define SR_CORE_ASSERT(x, ...)  {if(x) {SR_LOG_ERROR("ASSERT ERROR:{0}", __VA_ARGS__); __debugbreak();}}
    #define SR_ASSERT(x, ...)       {if(x) {APPLICATION_LOG_ERROR("ASSERT ERROR:{0}", __VA_ARGS__); __debugbreak();}}
#else
    #define SR_CORE_ASSERT(x, ...)
    #define SR_ASSERT(x, ...)
#endif // SR_ASSERT_ENABLE




