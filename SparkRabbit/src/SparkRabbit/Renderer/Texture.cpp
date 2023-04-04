#include "PrecompileH.h"

#include "Texture.h"

#include "RenderAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace SparkRabbit {
	uint32_t Texture::GetBPP(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB:		return 3;
		case TextureFormat::RGBA:		return 4;
		case TextureFormat::Float16:	return 8; //Represent for GL_RGBA16F for OpenGL, 16 bits fro each channel, so 8 bytes for each pixel
		}
		SPARK_CORE_ASSERT(false, "Unknown TextureFormat!");
		return 0;
		
	}

	uint32_t Texture::CalculateMipMapCount(uint32_t width, uint32_t height)
	{
		uint32_t levels = 1;
		while ((width | height) >> levels)
			levels++;

		return levels;
	}

	std::shared_ptr<Texture2D> Texture2D::Create(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap)
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:   return nullptr;
			case RenderAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(format, width, height, wrap);
		}

		SPARK_CORE_ASSERT(false, "Unsupported Render API!");
		return nullptr;
	}

	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path, bool srgb)
	{
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:   return nullptr;
			case RenderAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path, srgb);
		}

		SPARK_CORE_ASSERT(false, "Unsupported Render API!");
		return nullptr;
	}

	std::shared_ptr<TextureCube> TextureCube::Create(TextureFormat format, uint32_t width, uint32_t height)
	{
		
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:   return nullptr;
			case RenderAPI::API:: OpenGL: return std::make_shared<OpenGLTextureCube>(format, width, height);
		}

		SPARK_CORE_ASSERT(false, "Unsupported Render API!"); 
		return nullptr;
	}

	std::shared_ptr<TextureCube> TextureCube::Create(const std::string& path)
	{
		switch (RenderAPI::GetAPI()) 
		{
			case RenderAPI::API::None:   return nullptr; 
			case RenderAPI::API::OpenGL: return std::make_shared<OpenGLTextureCube>(path); 
		}

		SPARK_CORE_ASSERT(false, "Unsupported Render API!"); 
		return nullptr;
		
	}
}