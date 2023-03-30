#include "PrecompileH.h"
#include "OpenGLFramebuffer.h"

#include "SparkRabbit/Renderer/Renderer.h"
#include <glad/glad.h>
namespace SparkRabbit
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		:m_Specification(spec)
	{
		Resize(spec.Width, spec.Height, true);
	}
	/*
	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		Renderer::Submit([this]() { glDeleteFramebuffers(1, &m_RendererID); });
	}*/

	void  OpenGLFramebuffer::Bind() const
	{
	}

	void  OpenGLFramebuffer::Unbind() const
	{
	}

	void  OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool recreate)
	{
		if(!recreate && (m_Specification.Width == width && m_Specification.Height == height))
			return;

		m_Specification.Width = width;
		m_Specification.Height = height;



	}

	void  OpenGLFramebuffer::BindTexture(uint32_t slot) const
	{
	}
}
 
