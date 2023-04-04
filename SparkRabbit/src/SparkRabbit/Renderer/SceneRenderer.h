#pragma once
#include "SparkRabbit/Scene/Scene.h"
#include "RenderPass.h"

namespace SparkRabbit {

	struct SceneRendererOptions
	{
		bool ShowGrid = true;
		bool ShowBoundingBoxes = false;
	};

	class SceneRenderer
	{
	public:
		static void Init();

		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const Scene* scene);
		static void EndScene();

		static void SubmitEntity(Entity* entity);

		static std::pair<std::shared_ptr<TextureCube>, std::shared_ptr<TextureCube>> CreateEnvironmentMap(const std::string& filepath);

		static std::shared_ptr<RenderPass> GetFinalRenderPass();
		static std::shared_ptr<Texture2D> GetFinalColorBuffer();

		// TODO: Temp
		static uint32_t GetFinalColorBufferRendererID();

		static SceneRendererOptions& GetOptions();
	private:
		static void FlushDrawList();
		static void GeometryPass();
		static void CompositePass();
	};
}
