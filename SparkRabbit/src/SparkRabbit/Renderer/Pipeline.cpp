#include"PrecompileH.h"
#include"Pipeline.h"
#include"Renderer.h"

#include"SparkRabbit/Renderer/RenderAPI.h"

namespace SparkRabbit {
	/*std::shared_ptr<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RenderAPI::GetAPI())
		{
		case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return std::make_shared<OpenGLRenderPass>(spec);
		}
		SPARK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}*/
}