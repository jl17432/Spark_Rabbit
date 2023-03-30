#include "PrecompileH.h"
#include "OpenGLTexture.h"

#include "SparkRabbit/Renderer/Renderer.h"
#include "SparkRabbit/Renderer/RenderAPI.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace SparkRabbit{
	static GLenum TextureFormatToGL(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB:		return GL_RGB;
		case TextureFormat::RGBA:		return GL_RGBA;
		case TextureFormat::Float16:	return GL_RGBA16F;
	
		}
		SPARK_CORE_ASSERT(false, "Unknown TextureFormat!");
		return 0;
	}


	// Texture2D-------------------------------------------------------------------------------


}



