#pragma once

#ifdef SR_PLATFORM_WINDOWS

extern SparkRabbit::Application* SparkRabbit::CreateApplication();


int main(int argc,char **argv) {

	SparkRabbit::Log::Init();
	SR_CORE_WARN("Initialized Log!");
	int a = 5;
	auto app = SparkRabbit::CreateApplication();
	app->Run();
	delete app;
}


#endif // SR_PLATFORM_WINDOWS