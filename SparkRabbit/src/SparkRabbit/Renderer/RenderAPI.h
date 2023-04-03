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


	enum class PrimitiveType
	{
		None = 0, Triangles, Lines
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


		static void Init();
		static void Shutdown();
	
		static void Clear(float r, float g, float b, float a);
		static void SetClearColor(float r, float g, float b, float a);

		static void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest = true);
		static void SetLineWidth(float width);

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}

