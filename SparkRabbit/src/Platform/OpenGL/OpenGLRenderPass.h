#pragma once

#include "SparkRabbit/Renderer/RenderPass.h"

namespace SparkRabbit {

	class OpenGLRenderPass : public RenderPass
	{
	public:
		OpenGLRenderPass(const RenderPassSpecification& spec);
		~OpenGLRenderPass() = default;

		virtual const RenderPassSpecification& GetSpecification() const override { return m_Specification; }


	private:
		RenderPassSpecification m_Specification;

	};

}

