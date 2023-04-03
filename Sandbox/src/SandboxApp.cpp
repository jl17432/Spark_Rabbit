#include <SparkAPI.h>
#include "EditorLayer.h"
#include "SparkRabbit/EntryPoint.h"



class Sandbox : public SparkRabbit::Application
{
public:
	Sandbox(const SparkRabbit::ApplicationProps& props)
		: SparkRabbit::Application(props)
	{
		
	}

	void OnInit() override
	{
		PushLayer(new SparkRabbit::EditorLayer());
	}

};

SparkRabbit::Application* SparkRabbit::CreateApplication()
{
	return new Sandbox({ "SparkRabbit Engine", 1600, 900 });
}