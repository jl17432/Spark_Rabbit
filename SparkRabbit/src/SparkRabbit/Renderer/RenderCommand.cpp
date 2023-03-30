#include "PrecompileH.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRenderWrapper.h"

namespace SparkRabbit {

	RenderAPI* RenderCommand::s_RenderAPI = new OpenGLRenderWrapper;
}

