#include "PrecompileH.h"
#include "Renderer.h"

#include "Shader.h"
#include <Glad/glad.h>

#include"RenderAPI.h"
//#include"SceneRenderer.h"
//#include "Renderer2D.h"

namespace SparkRabbit {
	void Renderer::Clear()
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  result = std::make_shared<OpenGLFramebuffer>(spec); break;
		}
	}
	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
	
}