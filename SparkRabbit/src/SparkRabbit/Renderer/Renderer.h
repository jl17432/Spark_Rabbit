#pragma once

#include "RenderCommand.h"
#include "RenderPass.h"
#include "Mesh.h"

namespace SparkRabbit {

	class Renderer
	{
	public:
		typedef void(*RenderCmdFn)(void*);

		//Commands
		static void Clear();
		



		///////////////////////
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }

	};


}