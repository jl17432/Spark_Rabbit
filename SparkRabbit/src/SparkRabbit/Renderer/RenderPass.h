#pragma once

#include "Framebuffer.h"

namespace SparkRabbit {

	struct RenderPassSpecification
	{
		std::shared_ptr<Framebuffer> TargetFramebuffer;
	};
	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual const RenderPassSpecification& GetSpecification() const = 0;

		static std::shared_ptr<RenderPass> Create(const RenderPassSpecification& spec);
	};
}