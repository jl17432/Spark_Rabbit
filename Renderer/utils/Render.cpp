#include "Render.h"
#include "Load.h"
#include "Shader.h"
#include "Texture.h"
#include "Version.h"
#include "VboHandler.h"
#include "ComputeNormal.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;

void initialization() {

	//初始化glfw
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW!" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);	//4x anti-alising
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// opengl -v3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_REFRESH_RATE, 30); //控制帧率

	// 创建窗口
	window = glfwCreateWindow(1280, 960, "My First Window", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to open a window!" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	// 初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	// 检测按键
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	//隐藏鼠标
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//设置鼠标位置
	glfwPollEvents();
	glfwSetCursorPos(window, 1280 / 2, 960 / 2);
	// 背景颜色
	glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
	// 深度测试
	glEnable(GL_DEPTH_TEST);
	// 如果fragment比前一个更靠近摄影机，则接受fragment
	glDepthFunc(GL_LESS);
	//裁剪
	//glDisable(GL_CULL_FACE);
}

void Render(const char* filePath, const char* texturePath, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents)
{
	//初始化
	initialization();

	// 创建VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// 从着色器创建并编译GLSL程序
	GLuint programID = LoadShaders("utils/MyVertexShader.vertexshader", "utils/MyFragmentShader.fragmentshader");

	// MVP uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint ModelView3x3MatrixID = glGetUniformLocation(programID, "MV3x3");

	// 读取材质
	GLuint DiffuseTexture = loadDDS("diffuse.DDS");
	GLuint NormalTexture = loadBMP_custom("normal.bmp");
	GLuint SpecularTexture = loadDDS("specular.DDS");

	// 采样器
	GLuint DiffuseTextureID = glGetUniformLocation(programID, "DiffuseTextureSampler");
	GLuint NormalTextureID = glGetUniformLocation(programID, "NormalTextureSampler");
	GLuint SpecularTextureID = glGetUniformLocation(programID, "SpecularTextureSampler");

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

	//创建优化输出
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	std::vector<glm::vec3> indexed_tangents;
	std::vector<glm::vec3> indexed_bitangents;

	// TBN 优化
	indexVBO_TBN(vertices, uvs, normals, tangents, bitangents, indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents);

	//load to a VBO=======================================

	// 顶点
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	// 材质
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	// 法线
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// 切线
	GLuint tangentbuffer;
	glGenBuffers(1, &tangentbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tangentbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

	// 副切线
	GLuint bitangentbuffer;
	glGenBuffers(1, &bitangentbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bitangentbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);

	//索引缓冲
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	// 光线
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	//glUseProgram(programID);
	//GLuint LightID2 = glGetUniformLocation(programID, "LightPosition_worldspace");

	// 帧数
	int Frames = 0;
	double lastTime = glfwGetTime();

	// 混合颜色(透明)
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 循环
	do
	{
		// 计算帧数
		double currentTime = glfwGetTime();
		Frames++;
		if (currentTime - lastTime >= 1.0)
		{
			std::cout << 1000.0 / double(Frames) << "ms/frame" << std::endl;
			Frames = 0;
			lastTime += 1.0;
		}

		// 清空屏幕
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 调用shader
		glUseProgram(programID);

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
		//glm::vec3 lightPos2 = glm::vec3(0, 0, -6);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		//glUniform3f(LightID2, lightPos2.x, lightPos2.y, lightPos2.z);

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

		// 画三角形
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

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	//clear shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &tangentbuffer);
	glDeleteBuffers(1, &bitangentbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	//glDeleteTextures(1, &DiffuseTexture);
	glDeleteTextures(1, &NormalTexture);
	//glDeleteTextures(1, &SpecularTexture);
	glDeleteVertexArrays(1, &VertexArrayID);

	//结束
	glfwTerminate();
	return;

}