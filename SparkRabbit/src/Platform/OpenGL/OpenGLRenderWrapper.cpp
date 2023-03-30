#include "PrecompileH.h"
#include "OpenGLRenderWrapper.h"


namespace SparkRabbit {

	void OpenGLRenderWrapper::Init()
	{
		
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLLogMessage, nullptr);

		/*
		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);*/

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  //启用立方体贴图的无缝过滤
		glEnable(GL_CULL_FACE);////
		glFrontFace(GL_CCW);  
		glCullFace(GL_BACK);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_MULTISAMPLE);

		auto& apiInfo = RenderAPI::GetInfo();
		apiInfo.vendor = (const char*)glGetString(GL_VENDOR);
		apiInfo.renderer = (const char*)glGetString(GL_RENDERER);
		apiInfo.version = (const char*)glGetString(GL_VERSION);

		glGetIntegerv(GL_MAX_SAMPLES, &apiInfo.MaxSamples);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &apiInfo.MaxAnisotropy);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &apiInfo.MaxTextureUnits);


	}
	void OpenGLRenderWrapper::Shutdown()
	{
	}
	void OpenGLRenderWrapper::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRenderWrapper::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLRenderWrapper::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
	void OpenGLRenderWrapper::SetDepthTest(bool enabled)
	{
		if (enabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
	
	void OpenGLRenderWrapper::OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			SPARK_CORE_CRITICAL("[OpenGL Debug Severity High] {0}", message); break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			SPARK_CORE_ERROR("[OpenGL Debug Severity Meduim] {0}", message); break;
		case GL_DEBUG_SEVERITY_LOW:
			SPARK_CORE_WARN("[OpenGL Debug Severity Low] {0}", message); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			SPARK_CORE_TRACE("[OpenGL Debug Severity Notification] {0}", message); break;
		}
	}


}
