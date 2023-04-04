#include "PrecompileH.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace SparkRabbit {


	std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size, VertexBufferUsage usage)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RenderAPI::API::OpenGL:  return std::make_shared<OpenGLVertexBuffer>(data, size, usage);
		}
		SPARK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RenderAPI::API::OpenGL:  return std::make_shared<OpenGLVertexBuffer>(size, usage);
		}
		SPARK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;

	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(void* data, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RenderAPI::API::OpenGL:  return std::make_shared<OpenGLIndexBuffer>(data, size);
		}
		SPARK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
		
	}

}