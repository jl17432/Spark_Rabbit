#pragma once

#include "RenderCommand.h"

namespace SparkRabbit {

	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }

	};


}