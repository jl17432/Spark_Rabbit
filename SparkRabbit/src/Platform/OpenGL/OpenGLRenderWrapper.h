#pragma once

#include "SparkRabbit/Renderer/RenderAPI.h"
#include <glad/glad.h>

namespace SparkRabbit {
	
	class OpenGLRenderWrapper : public RenderAPI
	{
	public:

		void Init() override;
		void Shutdown() override;

		void SetClearColor(const glm::vec4& color) override;

		void Clear() override;

		void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;

		virtual void SetDepthTest(bool enabled) override;

		static void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}

