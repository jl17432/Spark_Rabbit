#include "PrecompileH.h"
#include "SceneRenderer.h"
#include"SceneEnvironment.h"

#include "Renderer.h"
#include "Renderer2D.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

namespace SparkRabbit {

	struct SceneRendererData
	{
		const Scene* ActiveScene = nullptr;
		struct SceneInfo
		{
			SceneRendererCamera SceneCamera;

			// Resources
			std::shared_ptr<MaterialInstance> SkyboxMaterial;
			std::shared_ptr<Environments> SceneEnvironment;
			Light ActiveLight;
		} SceneData;

		std::shared_ptr<Texture2D> BRDFLUT;
		std::shared_ptr<Shader> CompositeShader;

		std::shared_ptr<RenderPass> GeoPass;
		std::shared_ptr<RenderPass> CompositePass;

		glm::vec2 FocusPoint = { 0.5f, 0.5f };
		float BloomThreshold = 1.5f;

		struct DrawCommand
		{
			std::shared_ptr<Mesh> Mesh;
			std::shared_ptr<MaterialInstance> Material;
			glm::mat4 Transform;
		};
		std::vector<DrawCommand> DrawList;
		std::vector<DrawCommand> SelectedMeshDrawList;
		std::vector<DrawCommand> ShadowPassDrawList;

		// Grid
		std::shared_ptr<MaterialInstance> GridMaterial;

		SceneRendererOptions Options;
	};

	static SceneRendererData s_Data;

	void SceneRenderer::Init()
	{
		FramebufferSpecification geoFramebufferSpec;
		geoFramebufferSpec.Width = 1280;
		geoFramebufferSpec.Height = 720;
		geoFramebufferSpec.Format = FramebufferFormat::RGBA16F;
		geoFramebufferSpec.Samples = 8;
		geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		RenderPassSpecification geoRenderPassSpec;
		geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
		s_Data.GeoPass = RenderPass::Create(geoRenderPassSpec);

		FramebufferSpecification compFramebufferSpec;
		compFramebufferSpec.Width = 1280;
		compFramebufferSpec.Height = 720;
		compFramebufferSpec.Format = FramebufferFormat::RGBA8;
		compFramebufferSpec.ClearColor = { 0.5f, 0.1f, 0.1f, 1.0f };

		RenderPassSpecification compRenderPassSpec;
		compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
		s_Data.CompositePass = RenderPass::Create(compRenderPassSpec);

		s_Data.CompositeShader = Shader::Create("assets/shaders/SceneComposite.glsl");
		s_Data.BRDFLUT = Texture2D::Create("assets/textures/BRDF_LUT.tga");

		// Grid
		auto gridShader = Shader::Create("assets/shaders/Grid.glsl");
		s_Data.GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
		float gridScale = 16.025f, gridSize = 0.025f;
		s_Data.GridMaterial->Set("u_Scale", gridScale);
		s_Data.GridMaterial->Set("u_Res", gridSize);
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		s_Data.GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height,false); 
		s_Data.CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height, false); 
	}

	void SceneRenderer::BeginScene(const Scene* scene, const SceneRendererCamera& camera)
	{
		SPARK_CORE_ASSERT(!s_Data.ActiveScene, "");

		s_Data.ActiveScene = scene;

		s_Data.SceneData.SceneCamera = camera;
		s_Data.SceneData.SkyboxMaterial = scene->m_SkyboxMaterial;
		s_Data.SceneData.SceneEnvironment = scene->m_Environment;
		s_Data.SceneData.ActiveLight = scene->m_Light;
	}

	void SceneRenderer::EndScene()
	{
		SPARK_CORE_ASSERT(s_Data.ActiveScene, "");

		s_Data.ActiveScene = nullptr;

		FlushDrawList();
	}

	void SceneRenderer::SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform, std::shared_ptr<MaterialInstance> overrideMaterial)
	{
		// TODO: Culling, sorting, etc.

		s_Data.DrawList.push_back({ mesh, overrideMaterial, transform });
		s_Data.ShadowPassDrawList.push_back({ mesh, overrideMaterial, transform });
	}

	void SceneRenderer::SubmitSelectedMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform)
	{
		s_Data.SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
		s_Data.ShadowPassDrawList.push_back({ mesh, nullptr, transform });
	}

	static std::shared_ptr<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;

	std::pair<std::shared_ptr<TextureCube>, std::shared_ptr<TextureCube>> SceneRenderer::CreateEnvironmentMap(const std::string& filepath)
	{
		const uint32_t cubemapSize = 2048;
		const uint32_t irradianceMapSize = 32;

		std::shared_ptr<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);
		if (!equirectangularConversionShader)
			equirectangularConversionShader = Shader::Create("assets/shaders/EquirectangularToCubeMap.glsl");
		std::shared_ptr<Texture2D> envEquirect = Texture2D::Create(filepath);
		SPARK_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");

		equirectangularConversionShader->Bind();
		envEquirect->Bind();
		Renderer::Submit([envUnfiltered, cubemapSize, envEquirect]()
			{
				glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
				glGenerateTextureMipmap(envUnfiltered->GetRendererID());
			});


		if (!envFilteringShader)
			envFilteringShader = Shader::Create("assets/shaders/EnvironmentMipFilter.glsl");

		std::shared_ptr<TextureCube> envFiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);

		Renderer::Submit([envUnfiltered, envFiltered]()
			{
				glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				envFiltered->GetWidth(), envFiltered->GetHeight(), 6);
			});

		envFilteringShader->Bind();
		envUnfiltered->Bind();

		Renderer::Submit([envUnfiltered, envFiltered, cubemapSize]() {
			const float deltaRoughness = 1.0f / glm::max((float)(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
			for (int level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount(); level++, size /= 2) // <= ?
			{
				const GLuint numGroups = glm::max(1, size / 32);
				glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
				glDispatchCompute(numGroups, numGroups, 6);
			}
			});

		if (!envIrradianceShader)
			envIrradianceShader = Shader::Create("assets/shaders/EnvironmentIrradiance.glsl");

		std::shared_ptr<TextureCube> irradianceMap = TextureCube::Create(TextureFormat::Float16, irradianceMapSize, irradianceMapSize);
		envIrradianceShader->Bind();
		envFiltered->Bind();
		Renderer::Submit([irradianceMap]()
			{
				glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
				glGenerateTextureMipmap(irradianceMap->GetRendererID());
			});

		return { envFiltered, irradianceMap };
	}

	void SceneRenderer::GeometryPass()
	{
		Renderer::BeginRenderPass(s_Data.GeoPass);

		auto& sceneCamera = s_Data.SceneData.SceneCamera;

		auto viewProjection = sceneCamera.Camera.GetProjectionMatrix() * sceneCamera.ViewMatrix;

		// Skybox
		auto skyboxShader = s_Data.SceneData.SkyboxMaterial->GetShader();
		s_Data.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
		Renderer::SubmitFullscreenQuad(s_Data.SceneData.SkyboxMaterial);

		// Render entities
		for (auto& dc : s_Data.DrawList)
		{
			auto baseMaterial = dc.Mesh->GetMaterial();
			baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
			baseMaterial->Set("u_CameraPosition", glm::inverse(s_Data.SceneData.SceneCamera.ViewMatrix)[3]);

			// Environment (TODO: don't do this per mesh)
			baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment->RadianceMap);
			baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment->IrradianceMap);
			baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

			// Set lights (TODO: move to light environment and don't do per mesh)
			baseMaterial->Set("lights", s_Data.SceneData.ActiveLight);

			auto overrideMaterial = nullptr; // dc.Material;
			Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
		}

		// Grid
		if (GetOptions().ShowGrid)
		{
			s_Data.GridMaterial->Set("u_ViewProjection", viewProjection);
			Renderer::SubmitQuad(s_Data.GridMaterial, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
		}

		if (GetOptions().ShowBoundingBoxes)
		{
			Renderer2D::BeginScene(viewProjection, true);
			for (auto& dc : s_Data.DrawList)
				Renderer::DrawBoundingBox(dc.Mesh, dc.Transform);
			Renderer2D::EndScene();
		}

		Renderer::EndRenderPass();
	}

	void SceneRenderer::CompositePass()
	{
		auto& compositeBuffer = s_Data.CompositePass->GetSpecification().TargetFramebuffer;

		Renderer::BeginRenderPass(s_Data.CompositePass);
		s_Data.CompositeShader->Bind();
		s_Data.CompositeShader->SetFloat("u_Exposure", s_Data.SceneData.SceneCamera.Camera.GetExposure());
		s_Data.CompositeShader->SetInt("u_TextureSamples", s_Data.GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
		s_Data.GeoPass->GetSpecification().TargetFramebuffer->BindTexture();
		s_Data.CompositeShader->SetFloat2("u_ViewportSize", glm::vec2(compositeBuffer->GetWidth(), compositeBuffer->GetHeight()));
		s_Data.CompositeShader->SetFloat2("u_FocusPoint", s_Data.FocusPoint);
		s_Data.CompositeShader->SetInt("u_TextureSamples", s_Data.GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
		s_Data.CompositeShader->SetFloat("u_BloomThreshold", s_Data.BloomThreshold);
		Renderer::Submit([]()
			{
				glBindTextureUnit(1, s_Data.GeoPass->GetSpecification().TargetFramebuffer->GetDepthAttachmentRendererID());
			});
		Renderer::SubmitFullscreenQuad(nullptr);
		Renderer::EndRenderPass();
	}

	void SceneRenderer::FlushDrawList()
	{
		SPARK_CORE_ASSERT(!s_Data.ActiveScene, "");

		GeometryPass();
		CompositePass();

		s_Data.DrawList.clear();
		s_Data.SceneData = {};
	}

	std::shared_ptr<Texture2D> SceneRenderer::GetFinalColorBuffer()
	{
		// return s_Data.CompositePass->GetSpecification().TargetFramebuffer;
		SPARK_CORE_ASSERT(false, "Not implemented");
		return nullptr;
	}

	std::shared_ptr<RenderPass> SceneRenderer::GetFinalRenderPass()
	{
		return s_Data.CompositePass;
	}

	uint32_t SceneRenderer::GetFinalColorBufferRendererID()
	{
		return s_Data.CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
	}

	void SceneRenderer::SetFocusPoint(const glm::vec2& point)
	{
		s_Data.FocusPoint = point;
	}

	SceneRendererOptions& SceneRenderer::GetOptions()
	{
		return s_Data.Options;
	}
}