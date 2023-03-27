#include "PrecompileH.h"
#include "Application.h"

#include <glad/glad.h>

#include "Input.h"

namespace SparkRabbit{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
#define vShaderPath  "../SparkRabbit/src/SparkRabbit/Shaders/shader.vert"
#define fShaderPath  "../SparkRabbit/src/SparkRabbit/Shaders/shader.frag"


	static GLenum OpenGLShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
			case SparkRabbit::ShaderDataType::Float:    return GL_FLOAT;
			case SparkRabbit::ShaderDataType::Vec2:		return GL_FLOAT;
			case SparkRabbit::ShaderDataType::Vec3:		return GL_FLOAT;
			case SparkRabbit::ShaderDataType::Vec4:		return GL_FLOAT;
			case SparkRabbit::ShaderDataType::Mat3:     return GL_FLOAT;
			case SparkRabbit::ShaderDataType::Mat4:     return GL_FLOAT;
			case SparkRabbit::ShaderDataType::Int:      return GL_INT;
			case SparkRabbit::ShaderDataType::Ivec2:    return GL_INT;
			case SparkRabbit::ShaderDataType::Ivec3:    return GL_INT;
			case SparkRabbit::ShaderDataType::Ivec4:    return GL_INT;
			case SparkRabbit::ShaderDataType::Bool:     return GL_BOOL;
		}

		SPARK_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		SPARK_CORE_ASSERT(!s_Instance,"Application already exists!")
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);


		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		{
			BufferLayout layout = {
				{ShaderDataType::Vec3, "a_Position"},
				{ShaderDataType::Vec4, "a_Color"}
			};

			m_VertexBuffer->SetLayout(layout);
		}

		uint32_t index = 0;
		const auto& layout = m_VertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				OpenGLShaderDataType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}


		uint32_t indices[3] = { 0, 1, 2 };
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));


		m_Shader.reset(new Shader(vShaderPath, fShaderPath));
	}

	Application::~Application()
	{

	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled) 
				break;
		}

	}

	
	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Shader->Bind();

			glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
			
			m_Window->OnUpdate(); //note: Window must update after layer!!!!!

		}
		
	}
	
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
