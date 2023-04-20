#pragma once
#include "SparkRabbit/Renderer/Framebuffer.h"

namespace SparkRabbit {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		//~OpenGLFramebuffer();
		~OpenGLFramebuffer();

		void Bind() const override;
		void Unbind() const override;
		virtual void Resize(uint32_t width, uint32_t height, bool recreate = false) override;

		void BindTexture(uint32_t slot = 0) const override;

		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }

		uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
		uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }
		uint32_t GetRendererID() const override { return m_RendererID; }

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		FramebufferSpecification m_Specification;
	};

}

