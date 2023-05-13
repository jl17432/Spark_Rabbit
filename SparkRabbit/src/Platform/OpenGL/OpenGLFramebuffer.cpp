#include "PrecompileH.h"
#include "OpenGLFramebuffer.h"

#include "SparkRabbit/Renderer/Renderer.h"
#include <glad/glad.h>
namespace SparkRabbit
{
	namespace Utils
	{
		// Determine the texture target type based on whether it is multisampled or not.
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		// Create OpenGL textures for rendering.
		static void CreateTextures(bool multisampled, RendererID* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), 1, outID);
		}

		// Bind the texture to the specified texture unit.
		static void BindTexture(bool multisampled, RendererID id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		// Determine the data type for the given format.
		static GLenum DataType(GLenum format)
		{
			switch (format)
			{
			case GL_RGBA8: return GL_UNSIGNED_BYTE;
			case GL_RG16F:
			case GL_RG32F:
			case GL_RGBA16F:
			case GL_RGBA32F: return GL_FLOAT;
			case GL_DEPTH24_STENCIL8: return GL_UNSIGNED_INT_24_8;
			}

			return 0;
		}

		// Attach a color texture to the framebuffer.
		static void AttachColorTexture(RendererID id, int samples, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				// Only RGBA access for now
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, DataType(format), nullptr);

				// Set the texture parameters.
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			// Attach the texture to the framebuffer.
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		// Attach a depth texture to the framebuffer.
		static void AttachDepthTexture(RendererID id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				// Set the texture parameters.
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			// Attach the texture to the framebuffer.
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		// Check if the given format is a depth format.
		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
			case FramebufferTextureFormat::DEPTH32F:
				return true;
			}
			return false;
		}
	}

	// OpenGL framebuffer class constructor.
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec), m_Width(spec.Width), m_Height(spec.Height)
	{
		// Initialize color and depth attachment formats based on provided specification.
		for (auto format : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(format.TextureFormat))
				m_ColorAttachmentFormats.emplace_back(format.TextureFormat);
			else
				m_DepthAttachmentFormat = format.TextureFormat;
		}

		Resize(spec.Width, spec.Height, true);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		// Submit a task to delete the framebuffer when it's no longer needed.
		Renderer::Submit([this]() {
			glDeleteFramebuffers(1, &this->m_RendererID);
			});
	}

	// Resize the framebuffer.
	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{
		// If the size hasn't changed and forceRecreate is not set, no need to resize.
		if (!forceRecreate && (m_Width == width && m_Height == height))
			return;

		m_Width = width;
		m_Height = height;


		// Submit a task to recreate the framebuffer and its attachments with the new size.
		Renderer::Submit([this]() mutable
			{
				GenerateFramebuffer(&this->m_RendererID, this->m_ColorAttachments, &this->m_DepthAttachment);

				bool multisample = this->m_Specification.Samples > 1;

				if (this->m_ColorAttachmentFormats.size())
				{
					this->m_ColorAttachments.resize(this->m_ColorAttachmentFormats.size());
					Utils::CreateTextures(multisample, this->m_ColorAttachments.data(), this->m_ColorAttachments.size());

					// Create color attachments
					for (int i = 0; i < this->m_ColorAttachments.size(); i++)
					{
						Utils::BindTexture(multisample, this->m_ColorAttachments[i]);
						switch (this->m_ColorAttachmentFormats[i])
						{
						case FramebufferTextureFormat::RGBA8:
							Utils::AttachColorTexture(this->m_ColorAttachments[i], this->m_Specification.Samples, GL_RGBA8, this->m_Width, this->m_Height, i);
							break;
						case FramebufferTextureFormat::RGBA16F:
							Utils::AttachColorTexture(this->m_ColorAttachments[i], this->m_Specification.Samples, GL_RGBA16F, this->m_Width, this->m_Height, i);
							break;
						case FramebufferTextureFormat::RGBA32F:
							Utils::AttachColorTexture(this->m_ColorAttachments[i], this->m_Specification.Samples, GL_RGBA32F, this->m_Width, this->m_Height, i);
							break;
						case FramebufferTextureFormat::RG32F:
							Utils::AttachColorTexture(this->m_ColorAttachments[i], this->m_Specification.Samples, GL_RG32F, this->m_Width, this->m_Height, i);
							break;
						}

					}
				}

				if (this->m_DepthAttachmentFormat != FramebufferTextureFormat::None)
				{
					Utils::CreateTextures(multisample, &this->m_DepthAttachment, 1);
					Utils::BindTexture(multisample, this->m_DepthAttachment);
					switch (this->m_DepthAttachmentFormat)
					{
					case FramebufferTextureFormat::DEPTH24STENCIL8:
						Utils::AttachDepthTexture(this->m_DepthAttachment, this->m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, this->m_Width, this->m_Height);
						break;
					case FramebufferTextureFormat::DEPTH32F:
						Utils::AttachDepthTexture(this->m_DepthAttachment, this->m_Specification.Samples, GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, this->m_Width, this->m_Height);
						break;
					}

				}

				if (this->m_ColorAttachments.size() > 1)
				{
					GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
					glDrawBuffers(this->m_ColorAttachments.size(), buffers);
				}
				else if (this->m_ColorAttachments.size() == 0)
				{
					// Only depth-pass
					glDrawBuffer(GL_NONE);
				}



				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			});
	}

	void OpenGLFramebuffer::Bind() const
	{
		Renderer::Submit([this]() {
			glBindFramebuffer(GL_FRAMEBUFFER, this->m_RendererID);
			glViewport(0, 0, this->m_Width, this->m_Height);
			});
	}

	void OpenGLFramebuffer::Unbind() const
	{
		Renderer::Submit([]() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			});
	}

	void OpenGLFramebuffer::BindTexture(uint32_t attachmentIndex, uint32_t slot) const
	{
		Renderer::Submit([this, attachmentIndex, slot]() {
			glBindTextureUnit(slot, this->m_ColorAttachments[attachmentIndex]);
		});
	}
	void OpenGLFramebuffer::GenerateFramebuffer(RendererID* rendererID, std::vector<RendererID>& colorAttachments, RendererID* depthAttachment)
	{
		if (*rendererID) 
		{
			glDeleteFramebuffers(1, rendererID); 
			glDeleteTextures(colorAttachments.size(), colorAttachments.data()); 
			glDeleteTextures(1, depthAttachment); 

			colorAttachments.clear(); 
			*depthAttachment = 0; 
		}

		glGenFramebuffers(1, rendererID);  
		glBindFramebuffer(GL_FRAMEBUFFER, *rendererID); 
	}

	OpenGLRenderPass::OpenGLRenderPass(const RenderPassSpecification& spec)
		: m_Specification(spec)
	{

	}
}
 
