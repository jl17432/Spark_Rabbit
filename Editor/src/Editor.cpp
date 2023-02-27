#include<SparkRabbit.h>
#include"imgui.h"
#include"EditorLayer.h"
#include"SparkRabbit/EntryPoint.h"

namespace SparkRabbit {
	class Editor : public Application
	{
	public:
		Editor() {
			PushLayer(new EditorLayer());
		}
		~Editor() {

		}


	};

	Application* CreateApplication() {
		return new Editor();
	}
}