#pragma once

#include "glm/glm.hpp"
#include "VertexArray.h"

namespace SparkRabbit {
	
	// TODO: move into separate header
	enum class PrimitiveType
	{
		None = 0, Triangles, Lines
	};

	struct RenderAPIInfo
	{
		std::string Vendor;
		std::string Renderer;
		std::string Version;

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
	
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;
		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}

