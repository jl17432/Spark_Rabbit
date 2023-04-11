#pragma once

#include "CmdQueue.h"
#include "RenderPass.h"
#include "Mesh.h"

namespace SparkRabbit {

	class ShaderLibrary;

	class Renderer
	{
	public:
		typedef void(*RenderCmdFn)(void*);

		//Commands

		static void Clear(float r, float g, float b, float a);

		static void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest = true);

		//Abstracted OpenGL function
		static void SetLineWidth(float width);
		static void Init();

		static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary();

		//Submit the rendering command to the rendering command queue buffer.
		template<typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCmd = [](void* ptr) {
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();

				// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
				// however some items like uniforms which contain std::strings still exist for now
				//static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
				pFunc->~FuncT();
			};
			auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		}

		static void WaitAndRender();

		//Renderer 
		static void BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass, bool clear = true);
		static void EndRenderPass();
		
		
		static void SubmitQuad(const std::shared_ptr<MaterialInstance>& material, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitFullscreenQuad(const std::shared_ptr<MaterialInstance>& material);
		static void SubmitMesh(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform, const std::shared_ptr<MaterialInstance>& overrideMaterial = nullptr);
		

		static void DrawBoundingBox(const BoundingBox& box, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawBoundingBox(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));
		

		inline static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }

	private:
		static CmdQueue& GetRenderCommandQueue();

	};


}