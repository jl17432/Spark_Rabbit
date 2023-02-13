#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv)
{
	Hazel::Log::Init();
	HZ_CORE_WARN("Initialized Log!");
	//HZ_CORE_FATAL("Initialized Log!");
	HZ_CORE_ERROR("Initialized Log!");
	HZ_CORE_TRACE("Initialized Log!");
	int a = 8;
	HZ_INFO("Hello! var = {0}", a);
	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif