#pragma once

#ifdef SR_PLATFORM_WINDOWS

extern SparkRabbit::Application* SparkRabbit::CreateApplication();

int main(int argc, char** argv)
{
	SparkRabbit::Log::Init();
	SPARK_CORE_TRACE("SparkRabbit Engine Initializing");

	auto app = SparkRabbit::CreateApplication();
	app->Run();
	delete app;
}

#endif