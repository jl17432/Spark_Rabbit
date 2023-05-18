#include "PrecompileH.h"
#include "VertexArray.h"
#include "SparkRabbit/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace SparkRabbit {
	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:   return nullptr;
		case RenderAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}
		SPARK_CORE_ASSERT(false, "Unsupported Render API!");
		return nullptr;
	}
}