#pragma once
#include "SparkRabbit/Scene/Scene.h"
#include "RenderPass.h"
#include"SparkRabbit/Renderer/Mesh.h"

namespace SparkRabbit {

	struct SceneRendererOptions
	{
		bool ShowGrid = true;
		bool ShowBoundingBoxes = false;
	};

	struct SceneRendererCamera
	{
		SparkRabbit::Camera Camera;
		glm::mat4 ViewMatrix;
		float Near, Far;
		float FOV;
	};

	class SceneRenderer
	{
	public:
		static void Init();

		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const Scene* scene, const SceneRendererCamera& camera);
		static void EndScene();

		static void SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), std::shared_ptr<MaterialInstance> overrideMaterial = nullptr);
		static void SubmitSelectedMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f));

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

		static void ShadowMapPass();
	};
}
