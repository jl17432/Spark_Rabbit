#include<SparkRabbit.h>
#include"imgui.h"

class ExampleLayer : public SparkRabbit::Layer
{
public:
	ExampleLayer() : Layer("Example"){}

	void update() override
	{
	}

	void onImguiRender() override
	{
		ImGui::Begin("test1");
		ImGui::Text("hello world");
		ImGui::End();
	}

	void onEvent(SparkRabbit::Event& event) override
	{

	}

};


class Sandbox : public SparkRabbit::Application
{
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}
	~Sandbox() {

	}


};

SparkRabbit::Application* SparkRabbit::CreateApplication() {
	return new Sandbox();
}