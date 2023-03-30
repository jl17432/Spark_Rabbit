#pragma once

#include "SparkRabbit/Renderer/RenderAPI.h"

namespace SparkRabbit {
	
	class OpenGLRenderWrapper : public RenderAPI
	{
	public:
		void SetClearColor(const glm::vec4& color) override;

		void Clear() override;

		void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	};
}

