#include "PrecompileH.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace SparkRabbit {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr; 
		case RenderAPI::API::OpenGL:  return new OpenGLVertexBuffer(vertices, size); 
		}

		SPARK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return new OpenGLIndexBuffer(indices, size);
		}

		SPARK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}