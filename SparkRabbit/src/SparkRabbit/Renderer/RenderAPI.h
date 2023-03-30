#pragma once

#include "glm/glm.hpp"
#include "VertexArray.h"

namespace SparkRabbit {
	
	struct RenderAPIInfo
	{
		std::string vendor;
		std::string renderer;
		std::string version;

		int MaxSamples = 0;
		float MaxAnisotropy = 0.0f;
		int MaxTextureUnits = 0;
	};
	
	class RenderAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL // ,Vulkan, DirectX
		};

		static RenderAPIInfo& GetInfo()
		{
			static RenderAPIInfo info;
			return info;
		}


		virtual void Init() = 0;
		virtual void Shutdown() = 0;
	
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;
		virtual void SetDepthTest(bool enabled) = 0;
		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}

