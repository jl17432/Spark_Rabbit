#pragma once

#ifdef SR_PLATFORM_WINDOWS

extern SparkRabbit::Application* SparkRabbit::CreateApplication();

int main(int argc, char** argv)
{
	SparkRabbit::Log::Init();
	SPARK_CORE_WARN("Initialized Log!");
	//HZ_CORE_FATAL("Initialized Log!");
	SPARK_CORE_ERROR("Initialized Log!");
	SPARK_CORE_TRACE("Initialized Log!");
	int a = 8;
	SPARK_INFO("Hello! var = {0}", a);
	auto app = SparkRabbit::CreateApplication();
	app->Run();
	delete app;
}

#endif