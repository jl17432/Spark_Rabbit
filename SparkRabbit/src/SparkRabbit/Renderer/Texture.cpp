#include "PrecompileH.h"

#include "Texture.h"

#include "RenderAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace SparkRabbit {
	
	std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		std::shared_ptr<Texture2D> result = nullptr;
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RenderAPI::API::OpenGL:  result = std::make_shared<OpenGLTexture2D>(width, height); break;
		}
		return result;
	}
	
	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path)
	{
		std::shared_ptr<Texture2D> result = nullptr;
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RenderAPI::API::OpenGL:  result = std::make_shared<OpenGLTexture2D>(path); break;
		}
		return result;
	}
	
}