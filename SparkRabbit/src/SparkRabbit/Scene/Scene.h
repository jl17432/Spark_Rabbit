#pragma once
#include "SparkRabbit/Renderer/ProjectiveCamera.h"
#include"SparkRabbit/UUID.h"
#include"SparkRabbit/TickTime.h"
#include"SparkRabbit/Renderer/Texture.h"
#include"SparkRabbit/Renderer/Material.h"
#include"SparkRabbit/Renderer/SceneEnvironment.h"
#include "SparkRabbit/Math/Math.h"
#include"SceneCamera.h"
#include<entt/include/entt.hpp>
// bullet3
#include "bullet3/include/bullet/btBulletDynamicsCommon.h"
#include "bullet3/include/bullet/LinearMath/btScalar.h"

class b2World;

namespace SparkRabbit {

	// Light struct for storing light attributes
	struct Light
	{
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };

		float Multiplier = 1.0f;
	};

	struct DirectionalLight
	{
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };
		float Multiplier = 0.0f;

		// C++ only
		bool CastShadows = true;
	};

	//for managing multiple Directional Lights
	struct LightEnvironment
	{
		DirectionalLight DirectionalLights[4];
	};

	class Entity;
	using EntityMap = std::unordered_map<UUID, Entity>;

	//for managing all entities in the scene and their components
	class Scene
	{
	public:
		Scene(const std::string& debugName = "Scene", bool isEditorScene = false);
		~Scene();

		void Init();
		void OnUpdate(TickTime ts);

		void OnEvent(Event& e);
		void Update2DBodies(TickTime ts);
		void Update3DBodies(TickTime ts);
		Camera* FindFirstCameraInScene();
		void OnRenderRuntime(TickTime ts);
		void OnRenderEditor(TickTime ts, const ProjectiveCamera& projectiveCamera);

		//Runtime update
		void UpdateCameraMovement(TickTime ts, Entity& cameraEntity);
		void SetupDirectionalLights();
		void RenderMeshesInScene(TickTime ts);

		// 2d physics
		void Init2DPhysics();
		// 3d physics
		void Init3DPhysics();
		void Create3DBody(Entity& entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void SetViewportSize(uint32_t width, uint32_t height);

		const std::shared_ptr<Environments>& GetEnvironment() const { return m_Environment; }

		void SetSkybox(const std::shared_ptr<TextureCube>& skybox);
		void SetupSkyLight();
		Light& GetLight() { return m_Light; }
		const Light& GetLight() const { return m_Light; }

		Entity GetMainCameraEntity();

		float& GetSkyboxLod() { return m_SkyboxLod; }

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "", bool runtimeMap = false);
		void DestroyEntity(Entity entity);

		void DuplicateEntity(Entity entity);

		template<typename T>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<T>();
		}

		Entity FindEntityByTag(const std::string& tag);
		Entity FindEntityByUUID(UUID id);

		glm::mat4 GetTransformRelativeToParent(Entity entity);

		const EntityMap& GetEntityMap() const { return m_EntityIDMap; }
		void CopyTo(std::shared_ptr<Scene>& target);

		UUID GetUUID() const { return m_SceneID; }

		// Editor-specific
		void SetSelectedEntity(entt::entity entity) { m_SelectedEntity = entity; }
	private:
		UUID m_SceneID;
		entt::entity m_SceneEntity;
		entt::registry m_Registry;

		std::string m_DebugName;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		EntityMap m_EntityIDMap;

		Light m_Light;
		float m_LightMultiplier = 0.3f;

		LightEnvironment m_LightEnvironment;

		std::shared_ptr<Environments> m_Environment;
		float m_EnvironmentIntensity = 1.0f;
		std::shared_ptr<TextureCube> m_SkyboxTexture;
		std::shared_ptr<MaterialInstance> m_SkyboxMaterial;

		entt::entity m_SelectedEntity;

		float m_SkyboxLod = 1.0f;
		bool m_IsPlaying = false;

		b2World* m_2Dworld = nullptr;

		btBroadphaseInterface* m_broadphase;
		btDefaultCollisionConfiguration* m_collisionConfiguration;
		btCollisionDispatcher* m_dispatcher;
		btSequentialImpulseConstraintSolver* m_solver;
		btDiscreteDynamicsWorld* m_dynamicsWorld;

		friend class Entity;
		friend class SceneRenderer;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}
