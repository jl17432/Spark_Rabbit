#include "PrecompileH.h"
#include "Scene.h"
#include"Components.h"
#include"Entity.h"
#include"SparkRabbit/Math/Math.h"

#include "SparkRabbit/Renderer/SceneRenderer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace SparkRabbit {

	static const std::string DefaultEntityName = "Entity";

	std::unordered_map<UUID, Scene*> s_ActiveScenes;

	struct SceneComponent
	{
		UUID SceneID;
	};

	// TODO: PHYSICS

	static void OnScriptComponentConstruct(entt::registry& registry, entt::entity entity)
	{
		auto sceneView = registry.view<SceneComponent>();
		UUID sceneID = registry.get<SceneComponent>(sceneView.front()).SceneID;

		Scene* scene = s_ActiveScenes[sceneID];
	}

	static void OnScriptComponentDestroy(entt::registry& registry, entt::entity entity)
	{
		auto sceneView = registry.view<SceneComponent>();
		UUID sceneID = registry.get<SceneComponent>(sceneView.front()).SceneID;

		Scene* scene = s_ActiveScenes[sceneID];
	}

	Scene::Scene(const std::string& debugName, bool isEditorScene)
		: m_DebugName(debugName)
	{
		m_Registry.on_construct<ScriptComponent>().connect<&OnScriptComponentConstruct>();
		m_Registry.on_destroy<ScriptComponent>().connect<&OnScriptComponentDestroy>();

		m_SceneEntity = m_Registry.create();
		m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);

		s_ActiveScenes[m_SceneID] = this;

		Init();
	}

	Scene::~Scene()
	{
		m_Registry.on_destroy<ScriptComponent>().disconnect();

		m_Registry.clear();
		s_ActiveScenes.erase(m_SceneID);
	}

	void Scene::Init()
	{
		auto skyboxShader = Shader::Create("assets/shaders/Skybox.glsl");
		m_SkyboxMaterial = MaterialInstance::Create(Material::Create(skyboxShader));
		m_SkyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);
	}

	//TODO: Scene::OnUpdate(TickTime ts), play mode

	//TODO: Scene::OnRenderRuntime(Timestep ts), pause mode

	//Editor mode
	void Scene::OnRenderEditor(TickTime ts, const ProjectiveCamera& projectiveCamera)
	{

		{
			m_LightEnvironment = LightEnvironment();
			auto lights = m_Registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
			uint32_t directionalLightIndex = 0;
			for (auto entity : lights)
			{
				auto [transformComponent, lightComponent] = lights.get<TransformComponent, DirectionalLightComponent>(entity);
				glm::vec3 direction = -glm::normalize(glm::mat3(transformComponent.GetTransform()) * glm::vec3(1.0f));
				m_LightEnvironment.DirectionalLights[directionalLightIndex++] =
				{
					direction,
					lightComponent.Radiance,
					lightComponent.Intensity,
					lightComponent.CastShadows
				};
			}
		}

		{
			m_Environment = std::shared_ptr<Environments>();
			auto lights = m_Registry.group<SkyLightComponent>(entt::get<TransformComponent>);
			for (auto entity : lights)
			{
				auto [transformComponent, skyLightComponent] = lights.get<TransformComponent, SkyLightComponent>(entity);
				m_Environment = skyLightComponent.SceneEnvironment;
				m_EnvironmentIntensity = skyLightComponent.Intensity;
				if (m_Environment)
					SetSkybox(m_Environment->RadianceMap);
			}
		}

		m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

		auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
		SceneRenderer::BeginScene(this, { projectiveCamera, projectiveCamera.GetView(), 0.1f, 1000.0f, 45.0f }); // TODO: real values
		for (auto entity : group)
		{
			auto& [meshComponent, transformComponent] = group.get<MeshComponent, TransformComponent>(entity);
			if (meshComponent.Mesh)
			{
				meshComponent.Mesh->OnUpdate(ts);

				glm::mat4 transform = GetTransformRelativeToParent(Entity{ entity, this });

				if (m_SelectedEntity == entity)
					SceneRenderer::SubmitSelectedMesh(meshComponent, transform);
				else
					SceneRenderer::SubmitMesh(meshComponent, transform);
			}
		}


		SceneRenderer::EndScene();
		/////////////////////////////////////////////////////////////////////

#if 0
		// Render all sprites
		Renderer2D::BeginScene(*camera);
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderer>);
			for (auto entity : group)
			{
				auto [transformComponent, spriteRendererComponent] = group.get<TransformComponent, SpriteRenderer>(entity);
				if (spriteRendererComponent.Texture)
					Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Texture, spriteRendererComponent.TilingFactor);
				else
					Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Color);
			}
		}

		Renderer2D::EndScene();
#endif
	}

	void Scene::OnEvent(Event& e)
	{
	}

	//TODO: RunTime start and stop

	void Scene::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	void Scene::SetSkybox(const std::shared_ptr <TextureCube>& skybox)
	{
		m_SkyboxTexture = skybox;
		m_SkyboxMaterial->Set("u_Texture", skybox);
	}

	Entity Scene::GetMainCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& comp = view.get<CameraComponent>(entity);
			if (comp.Primary)
				return { entity, this };
		}
		return {};
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = {};

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);

		entity.AddComponent<RelationshipComponent>();

		m_EntityIDMap[idComponent.ID] = entity;
		return entity;
	}

	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name, bool runtimeMap)
	{
		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = uuid;

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);

		entity.AddComponent<RelationshipComponent>();

		SPARK_CORE_ASSERT(false,m_EntityIDMap.find(uuid) == m_EntityIDMap.end());
		m_EntityIDMap[uuid] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity.m_EntityHandle);
	}

	template<typename T>
	static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto components = srcRegistry.view<T>();
		for (auto srcEntity : components)
		{
			entt::entity destEntity = enttMap.at(srcRegistry.get<IDComponent>(srcEntity).ID);

			auto& srcComponent = srcRegistry.get<T>(srcEntity);
			auto& destComponent = dstRegistry.emplace_or_replace<T>(destEntity, srcComponent);
		}
	}

	template<typename T>
	static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry& registry)
	{
		if (registry.has<T>(src))
		{
			auto& srcComponent = registry.get<T>(src);
			registry.emplace_or_replace<T>(dst, srcComponent);
		}
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity;
		if (entity.HasComponent<TagComponent>())
			newEntity = CreateEntity(entity.GetComponent<TagComponent>().Tag);
		else
			newEntity = CreateEntity();

		CopyComponentIfExists<TransformComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<RelationshipComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<MeshComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<DirectionalLightComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<SkyLightComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<ScriptComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<CameraComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
	}

	Entity Scene::FindEntityByTag(const std::string& tag)
	{
		// TODO: If this becomes used often, consider indexing by tag
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const auto& canditate = view.get<TagComponent>(entity).Tag;
			if (canditate == tag)
				return Entity(entity, this);
		}

		return Entity{};
	}

	Entity Scene::FindEntityByUUID(UUID id)
	{
		auto view = m_Registry.view<IDComponent>();
		for (auto entity : view)
		{
			auto& idComponent = m_Registry.get<IDComponent>(entity);
			if (idComponent.ID == id)
				return Entity(entity, this);
		}

		return Entity{};
	}

	glm::mat4 Scene::GetTransformRelativeToParent(Entity entity)
	{
		glm::mat4 transform(1.0F);

		Entity parent = FindEntityByUUID(entity.GetParentUUID());
		if (parent)
			transform = GetTransformRelativeToParent(parent);

		return transform * entity.Transform().GetTransform();
	}

	// Copy to runtime
	void Scene::CopyTo(std::shared_ptr<Scene>& target)
	{
		// Environment
		target->m_Light = m_Light;
		target->m_LightMultiplier = m_LightMultiplier;

		target->m_Environment = m_Environment;
		target->m_SkyboxTexture = m_SkyboxTexture;
		target->m_SkyboxMaterial = m_SkyboxMaterial;
		target->m_SkyboxLod = m_SkyboxLod;

		std::unordered_map<UUID, entt::entity> enttMap;
		auto idComponents = m_Registry.view<IDComponent>();
		for (auto entity : idComponents)
		{
			auto uuid = m_Registry.get<IDComponent>(entity).ID;
			Entity e = target->CreateEntityWithID(uuid, "", true);
			enttMap[uuid] = e.m_EntityHandle;
		}

		CopyComponent<TagComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<TransformComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<RelationshipComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<MeshComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<DirectionalLightComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<SkyLightComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<ScriptComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<CameraComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<SpriteRendererComponent>(target->m_Registry, m_Registry, enttMap);

	}
}
