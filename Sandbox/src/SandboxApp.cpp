#include<SparkRabbit.h>

class ExampleLayer : public SparkRabbit::Layer
{
public:
	ExampleLayer() : Layer("Example"){}

	void update() override
	{
		SR_INFO("ExampleLayer::Update");
	}

	void onEvent(SparkRabbit::Event& event) override
	{
		SR_TRACE("{0}", event);
	}

};


class Sandbox : public SparkRabbit::Application
{
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		PushEndLayer(new SparkRabbit::ImguiLayer());
	}
	~Sandbox() {

	}


};

SparkRabbit::Application* SparkRabbit::CreateApplication() {
	return new Sandbox();
}