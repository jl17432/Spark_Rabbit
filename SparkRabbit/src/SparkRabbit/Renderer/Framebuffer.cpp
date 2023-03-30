#include "PrecompileH.h"
#include "Framebuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace SparkRabbit {

	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		std::shared_ptr<Framebuffer> result = nullptr;
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RenderAPI::API::OpenGL:  result = std::make_shared<OpenGLFramebuffer>(spec); break;
		}
		FramebufferPool::GetGlobal()->Add(result);
		return result;
	}
	
	std::weak_ptr<Framebuffer> FramebufferPool::AllocateBuffer()
	{
		return std::weak_ptr<Framebuffer>();
	}
	void FramebufferPool::Add(std::weak_ptr<Framebuffer> framebuffer)
	{
		m_Pool.emplace_back(framebuffer);
	}
}