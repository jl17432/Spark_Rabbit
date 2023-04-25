#pragma once

#include <glm/glm.hpp>

#include "SparkRabbit/Renderer/RenderAPI.h"

namespace SparkRabbit {

	enum class FramebufferFormat
	{
		None = 0,

		// Color
		RGBA8 = 1,
		RGBA16F = 2,
		RGBA32F = 3,
		RG32F = 4,

		// Depth/stencil
		DEPTH32F = 5,
		DEPTH24STENCIL8 = 6,

		// Defaults
		Depth = DEPTH24STENCIL8
	};


	
	struct FramebufferSpecification
	{
		uint32_t Width = 1280;
		uint32_t Height = 720;
		glm::vec4 ClearColor;
		FramebufferFormat Format;
		uint32_t Samples = 1; // multisampling

		bool NoResize = false;

		// SwapChainTarget = screen buffer (i.e. no framebuffer)
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Resize(uint32_t width, uint32_t height, bool recreate) = 0;

		virtual void BindTexture(uint32_t slot = 0) const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetRendererID() const = 0;
		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);

	};

	class FramebufferPool final
	{
	public:
		FramebufferPool(uint32_t maxFBs = 32);
		~FramebufferPool();

		std::weak_ptr<Framebuffer> AllocateBuffer();
		void Add(std::weak_ptr<Framebuffer> framebuffer);

		const std::vector<std::weak_ptr<Framebuffer>>& GetAll() const { return m_Pool; }

		inline static FramebufferPool* GetGlobal() { return s_Instance; }
	private:
		std::vector<std::weak_ptr<Framebuffer>> m_Pool;

		static FramebufferPool* s_Instance;
	};

}

