#include "PrecompileH.h"
#include "RenderPass.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLRenderPass.h"

namespace SparkRabbit{
	std::shared_ptr<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RenderAPI::API::OpenGL:  return std::make_shared<OpenGLRenderPass>(spec);
		}
		SPARK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}