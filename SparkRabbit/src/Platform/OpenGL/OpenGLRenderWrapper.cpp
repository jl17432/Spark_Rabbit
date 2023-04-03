#include "PrecompileH.h"
#include "SparkRabbit/Renderer/RenderAPI.h"

#include<Glad/glad.h>

namespace SparkRabbit {

	static void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			SPARK_CORE_ERROR("[OpenGL Debug HIGH] {0}", message);
			//SPARK_CORE_ASSERT(false, "GL_DEBUG_SEVERITY_HIGH");
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			SPARK_CORE_WARN("[OpenGL Debug MEDIUM] {0}", message);
			break;
		case GL_DEBUG_SEVERITY_LOW:
			SPARK_CORE_INFO("[OpenGL Debug LOW] {0}", message);
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			break;
		}
	}
	
	void RenderAPI::Init() 
	{
		glDebugMessageCallback(OpenGLLogMessage, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glFrontFace(GL_CCW);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_MULTISAMPLE);

		auto& caps = RenderAPI::GetInfo();

		caps.vendor = (const char*)glGetString(GL_VENDOR);
		caps.renderer = (const char*)glGetString(GL_RENDERER);
		caps.version = (const char*)glGetString(GL_VERSION);

		glGetIntegerv(GL_MAX_SAMPLES, &caps.MaxSamples);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &caps.MaxAnisotropy);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &caps.MaxTextureUnits);
		/*
		GLenum error = glGetError();
		while (error != GL_NO_ERROR)
		{
			SPARK_CORE_ERROR("OpenGL Error {0}", error);
			error = glGetError();
		}*/

		//LoadRequiredAssets(); 
	}

	void RenderAPI::Shutdown()
	{
		//UnloadRequiredAssets(); 
	}

	void RenderAPI::Clear(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void RenderAPI::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
	{
		if (!depthTest)
			glDisable(GL_DEPTH_TEST);
		
		switch (type)
		{
		case PrimitiveType::Triangles:
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
			break;
		case PrimitiveType::Lines:
			glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, nullptr);
			break;
		}
		if (!depthTest)
			glEnable(GL_DEPTH_TEST);
	}

	void RenderAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}



}
