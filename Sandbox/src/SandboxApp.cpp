#include <SparkAPI.h>


class ExampleLayer : public SparkRabbit::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{ 
		//SPARK_INFO("ExampleLayer::Update");
		if (SparkRabbit::Input::IsKeyPressed(SR_KEY_A))
			SPARK_INFO("A key is pressed!");
	}

	void OnEvent(SparkRabbit::Event& event) override
	{
		//SPARK_TRACE("{0}", event);
		if (event.GetEventType() == SparkRabbit::EventType::KeyPressed)
		{
			SparkRabbit::KeyPressedEvent& e = (SparkRabbit::KeyPressedEvent&)event;
			SPARK_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

};

class Sandbox : public SparkRabbit::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

SparkRabbit::Application* SparkRabbit::CreateApplication()
{
	return new Sandbox();
}