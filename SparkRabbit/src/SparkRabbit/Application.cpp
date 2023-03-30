#include "PrecompileH.h"
#include "Application.h"

#include "SparkRabbit/Renderer/Renderer.h"

#include "Input.h"

namespace SparkRabbit{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
#define ShaderPath_  "../SparkRabbit/src/SparkRabbit/Shaders/"



	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		SPARK_CORE_ASSERT(!s_Instance,"Application already exists!")
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_VertexArray.reset(VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		BufferLayout layout = {
			{ShaderDataType::Vec3, "a_Position"},
			{ShaderDataType::Vec4, "a_Color"}
		};
		
		vertexBuffer->SetLayout(layout); 
		m_VertexArray->AddVertexBuffer(vertexBuffer);  

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer); 

		m_Shader.reset(new Shader(ShaderPath_"shader.vert", ShaderPath_"shader.frag"));


		m_SquareVA.reset(VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<VertexBuffer> squareVB; 
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices))); 
		squareVB->SetLayout({
			{ ShaderDataType::Vec3, "a_Position" } 
			});
		m_SquareVA->AddVertexBuffer(squareVB); 

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 }; 
		std::shared_ptr<IndexBuffer> squareIB; 
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t))); 
		m_SquareVA->SetIndexBuffer(squareIB); 


		m_BlueShader.reset(new Shader(ShaderPath_"square.vert", ShaderPath_"square.frag"));

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
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			Renderer::BeginScene();


			m_BlueShader->Bind();
			Renderer::Submit(m_SquareVA);

			m_Shader->Bind();
			Renderer::Submit(m_VertexArray);

			Renderer::EndScene();


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
