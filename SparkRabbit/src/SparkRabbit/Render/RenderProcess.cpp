#include "PreCompile.h"
#include "RenderProcess.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include"PlatForm/WindowsWindow.h"
#include"SparkRabbit/Application.h"


namespace SparkRabbit {

	void Render(const char* filePath, const char* texturePath, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals)
	{
		auto window = static_cast<GLFWwindow*>(SparkRabbit::Application::Get().GetWindow().GetNativeWindow());
		// ArrayID
		GLuint VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		// 从着色器创建并编译GLSL程序
		GLuint programID = LoadShaders("D:/leeds/SparkRabbit/SparkRabbit/src/SparkRabbit/Render/MyVertexShader.vertexshader", "D:/leeds/SparkRabbit/SparkRabbit/src/SparkRabbit/Render/MyFragmentShader.fragmentshader");

		// Get a handle for our "MVP" uniform
		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
		GLuint ModelMatrixID = glGetUniformLocation(programID, "M");


		// 材质
		//GLuint Texture = loadBMP_custom("bmptest.bmp");
		GLuint Texture = loadDDS("D:/leeds/SparkRabbit/SparkRabbit/src/SparkRabbit/uvmap.dds");
		GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

		// 读文件
		bool load_success = LoadObjModel(filePath, vertices, uvs, normals);
		if (load_success == false)
		{
			fprintf(stderr, "Failed to read obj file!");
			glfwTerminate();
		}

		std::vector<unsigned short> indices;
		std::vector<glm::vec3> indexed_vertices;
		std::vector<glm::vec2> indexed_uvs;
		std::vector<glm::vec3> indexed_normals;
		indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

		// 创建顶点buffer
		GLuint vertexbuffer;
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

		// 颜色或材质
		GLuint uvbuffer;
		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

		// normal buffer
		GLuint normalbuffer;
		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

		// Generate a buffer for the indices as well
		GLuint elementbuffer;
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

		// Get a handle for our "LightPosition" uniform
		glUseProgram(programID);
		GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");


		// 帧数
		double lastTime = glfwGetTime();
		int nbFrames = 0;


		// 循环
		do
		{
			// 计算帧数
			double currentTime = glfwGetTime();
			nbFrames++;
			if (currentTime - lastTime >= 1.0)
			{
				std::cout << 1000.0 / double(nbFrames) << "ms/frame" << std::endl;
				nbFrames = 0;
				lastTime += 1.0;
			}

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


		}

		//close window
		while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

		//clear
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteBuffers(1, &uvbuffer);
		glDeleteBuffers(1, &normalbuffer);
		glDeleteBuffers(1, &elementbuffer);
		glDeleteProgram(programID);
		glDeleteTextures(1, &Texture);
		glDeleteVertexArrays(1, &VertexArrayID);

		//结束
		glfwTerminate();
		return;
	}
}
