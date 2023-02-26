#include"PreCompile.h"
#include "Application.h"

#include"glad/glad.h"
#include"EventsSystem/WindowEvent.h"
#include"Log.h"
#include"SparkRabbit/input.h"
#include"Render/RenderProcess.h"

GLuint VertexArrayID, programID, MatrixID, ViewMatrixID, ModelMatrixID,
Texture, TextureID, vertexbuffer, uvbuffer, normalbuffer, elementbuffer, LightID;

std::vector<unsigned short> indices;
std::vector<glm::vec3> indexed_vertices;
std::vector<glm::vec2> indexed_uvs;
std::vector<glm::vec3> indexed_normals;

namespace SparkRabbit {

	Application* Application::s_Instance = nullptr;


	Application::Application() {
		SR_CORE_ASSERT(s_Instance, "Application is already exit");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		auto BindFunc = std::bind(&Application::OnEvent, this, std::placeholders::_1);
		m_Window->EventCallBack(BindFunc);

		m_ImguiLayer = new ImguiLayer();
		PushEndLayer(m_ImguiLayer);

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		const char* filePath = "D:/leeds/SparkRabbit/SparkRabbit/src/SparkRabbit/m4a1_s.obj";
		const char* texturePath = "D:/leeds/SparkRabbit/SparkRabbit/src/SparkRabbit/uvmap.dds";

		//Render(filePath, texturePath, vertices, uvs, normals);
		// ArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		// 从着色器创建并编译GLSL程序
		programID = LoadShaders("D:/leeds/SparkRabbit/SparkRabbit/src/SparkRabbit/Render/MyVertexShader.vertexshader", "D:/leeds/SparkRabbit/SparkRabbit/src/SparkRabbit/Render/MyFragmentShader.fragmentshader");

		// Get a handle for our "MVP" uniform
		MatrixID = glGetUniformLocation(programID, "MVP");
		ViewMatrixID = glGetUniformLocation(programID, "V");
		ModelMatrixID = glGetUniformLocation(programID, "M");


		// 材质
		//GLuint Texture = loadBMP_custom("bmptest.bmp");
		Texture = loadDDS("D:/leeds/SparkRabbit/SparkRabbit/src/SparkRabbit/uvmap.dds");
		TextureID = glGetUniformLocation(programID, "myTextureSampler");

		// 读文件
		bool load_success = LoadObjModel(filePath, vertices, uvs, normals);
		if (load_success == false)
		{
			fprintf(stderr, "Failed to read obj file!");
			glfwTerminate();
		}

		indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

		// 创建顶点buffer
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

		// 颜色或材质
		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

		// normal buffer
		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

		// Generate a buffer for the indices as well
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

		// Get a handle for our "LightPosition" uniform
		glUseProgram(programID);
		LightID = glGetUniformLocation(programID, "LightPosition_worldspace");


		// 帧数
		double lastTime = glfwGetTime();
		int nbFrames = 0;

	}

	Application::~Application() {

	}

	void Application::PushLayer(Layer* layer)
	{
		m_Layerstack.PushLayer(layer);
		layer->OnAttach();

	}

	void Application::PopLayer(Layer* layer)
	{
		m_Layerstack.PopLayer(layer);

	}

	void Application::PushEndLayer(Layer* endlayer)
	{
		m_Layerstack.PushEndLayer(endlayer);
		endlayer->OnAttach();
	}

	void Application::PopEndLayer(Layer* endlayer)
	{
		m_Layerstack.PopEndLayer(endlayer);
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

		for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();)
		{
			(*--it)->onEvent(event);
			if (event.GetStatus())
			{
				break;
			}
		}
	}

	void Application::Run() {
		while (m_running)
		{
			// 清空屏幕
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 调用shader
			glUseProgram(programID);

			//GLuint MatrixID = glGetUniformLocation(programID, "MVP");

			computeMatricesFromInputs();
			// MVP矩阵
			glm::mat4 ProjectionMatrix = getProjectionMatrix();
			glm::mat4 ViewMatrix = getViewMatrix();
			glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

			// 变换发送到着色器
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// light source attributes
			glm::vec3 lightPos = glm::vec3(4, 4, 4);
			glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);


			// 绑定纹理
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture);
			glUniform1i(TextureID, 0);

			// buffer里vertices结构
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 纹理buffer结构
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// 3rd attribute buffer : normals
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
			glVertexAttribPointer(
				2,                                // attribute
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);


			// Index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

			// Generate a buffer for the indices as well
			GLuint elementbuffer;
			glGenBuffers(1, &elementbuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

			////画
			//glDrawArrays(GL_TRIANGLES, 0, vertices.size());

			// Draw the triangles !
			glDrawElements(
				GL_TRIANGLES,      // mode
				indices.size(),    // count
				GL_UNSIGNED_SHORT,   // type
				(void*)0           // element array buffer offset
			);

			//清理
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);

			for (Layer* layer : m_Layerstack)
			{
				layer->update();
			}

			//Imgui
			m_ImguiLayer->begin();
			for (Layer* layer : m_Layerstack)
			{
				layer->onImguiRender();
			}
			m_ImguiLayer->end();

			
			m_Window->Update();
		}
	} 

	void Application::Close()
	{
		m_running = false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_running = false;
		return true;
	}
	
}
