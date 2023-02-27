#include"PreCompile.h"
#include "Application.h"

#include"glad/glad.h"
#include"EventsSystem/WindowEvent.h"
#include"Log.h"
#include<glm.hpp>
#include"SparkRabbit/input.h"
#include"SparkRabbit/Render/Control.h"
#include"SparkRabbit/Render/Shader.h"
#include"SparkRabbit/Render/Texture.h"
#include"SparkRabbit/Render/Load.h"
#include"SparkRabbit/Render/VBOHandler.h"
#include"SparkRabbit/Render/ComputeNormal.h"

GLuint VertexArrayID, programID, MatrixID, ViewMatrixID, ModelMatrixID, 
ModelView3x3MatrixID, DiffuseTexture, NormalTexture, SpecularTexture, 
DiffuseTextureID, NormalTextureID,SpecularTextureID, tangentbuffer, bitangentbuffer, 
Texture, TextureID, vertexbuffer, uvbuffer, normalbuffer, elementbuffer, LightID;

std::vector<unsigned short> indices;
std::vector<glm::vec3> indexed_vertices;
std::vector<glm::vec2> indexed_uvs;
std::vector<glm::vec3> indexed_normals;
std::vector<glm::vec3> indexed_tangents;
std::vector<glm::vec3> indexed_bitangents;

#define ShaderDir_  "../SparkRabbit/src/SparkRabbit/Render/Shaders/"
#define ResourceDir_ "../SparkRabbit/src/SparkRabbit/Resource/"

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
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;

		const char* filePath = ResourceDir_"cylinder.obj";
		



		// 创建VAO
		//GLuint VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		// 从着色器创建并编译GLSL程序
		programID = LoadShaders(ShaderDir_"MyVertexShader.vertexshader", ShaderDir_"MyFragmentShader.fragmentshader");

		// MVP uniform
		MatrixID = glGetUniformLocation(programID, "MVP");
		ViewMatrixID = glGetUniformLocation(programID, "V");
		ModelMatrixID = glGetUniformLocation(programID, "M");
		ModelView3x3MatrixID = glGetUniformLocation(programID, "MV3x3");

		// 读取材质
		DiffuseTexture = loadDDS(ResourceDir_"diffuse.DDS");
		NormalTexture = loadBMP_custom(ResourceDir_"normal.bmp");
		SpecularTexture = loadDDS(ResourceDir_"specular.DDS");

		// 采样器
		DiffuseTextureID = glGetUniformLocation(programID, "DiffuseTextureSampler");
		NormalTextureID = glGetUniformLocation(programID, "NormalTextureSampler");
		SpecularTextureID = glGetUniformLocation(programID, "SpecularTextureSampler");

		// 读文件
		bool load_success = LoadObjModel(filePath, vertices, uvs, normals);
		
		// 读取不成功
		if (load_success == false)
		{
			fprintf(stderr, "Failed to read obj file!");
			glfwTerminate();
		}

		// 计算切线，副切线
		computeTangentBasis(
			vertices, uvs, normals, // input
			tangents, bitangents    // output
		);

		////创建优化输出
		//std::vector<unsigned short> indices;
		//std::vector<glm::vec3> indexed_vertices;
		//std::vector<glm::vec2> indexed_uvs;
		//std::vector<glm::vec3> indexed_normals;
		//std::vector<glm::vec3> indexed_tangents;
		//std::vector<glm::vec3> indexed_bitangents;

		// TBN 优化
		indexVBO_TBN(vertices, uvs, normals, tangents, bitangents, indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents);
		
		//load to a VBO=======================================

		// 顶点
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

		// 材质
		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

		// 法线
		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

		// 切线
		glGenBuffers(1, &tangentbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tangentbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

		// 副切线
		glGenBuffers(1, &bitangentbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bitangentbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);

		//索引缓冲
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);


		// 光线
		glUseProgram(programID);
		LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

		//glUseProgram(programID);
		//GLuint LightID2 = glGetUniformLocation(programID, "LightPosition_worldspace");

		// 帧数
		int Frames = 0;
		double lastTime = glfwGetTime();

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
			// Çå¿ÕÆÁÄ»
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// µ÷ÓÃshader
			glUseProgram(programID);

			//GLuint MatrixID = glGetUniformLocation(programID, "MVP");

			// 计算矩阵
			computeMatricesFromInputs();
			glm::mat4 ProjectionMatrix = getProjectionMatrix();
			glm::mat4 ViewMatrix = getViewMatrix();
			glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
			glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

			// 变换矩阵发送到着色器
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
			glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

			// 光源位置
			glm::vec3 lightPos = glm::vec3(0, 0, 4);
			glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);


			// 与缓冲区一一对应
		    // 绑定纹理, 漫反射用 Unit0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
			glUniform1i(DiffuseTextureID, 0);

			// 法线用 Unit1
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, NormalTexture);
			glUniform1i(NormalTextureID, 1);

			// 镜反射 Unit2
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, SpecularTexture);
			glUniform1i(SpecularTextureID, 2);

			// vertices结构
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

			// 纹理结构
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

			// 法线结构
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

			// 切线结构
			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
			glVertexAttribPointer(
				3,                                // attribute
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// 副切线结构
			glEnableVertexAttribArray(4);
			glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
			glVertexAttribPointer(
				4,                                // attribute
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

			

			////»­
			//glDrawArrays(GL_TRIANGLES, 0, vertices.size());

			// Draw the triangles !
			glDrawElements(
				GL_TRIANGLES,      // mode
				indices.size(),    // count
				GL_UNSIGNED_SHORT,   // type
				(void*)0           // element array buffer offset
			);

			// 关闭Array
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			glDisableVertexAttribArray(4);

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
