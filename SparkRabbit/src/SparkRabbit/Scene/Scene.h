#pragma once
#include "Entity.h"
#include "SparkRabbit/Renderer/ProjectiveCamera.h"

namespace SparkRabbit {
	struct Environment
	{
		std::shared_ptr<TextureCube> RadianceMap;
		std::shared_ptr<TextureCube> IrradianceMap;

		static Environment Load(const std::string& path);
	};

	struct Light  //Directional light for now  //Todo - Point light, Spot light(important for FPS game)
	{
		glm::vec3 Direction = glm::vec3(0.0f);
		glm::vec3 Radiance;

		float Multiplier = 1.0f;
	};

	class Scene
	{
	public:
		Scene(const std::string& name = "Scene");
		~Scene();

		void Init();

		void OnUpdate(TickTime ts);
		void OnEvent(Event& e);

		void SetCamera(const ProjectiveCamera& camera);
		ProjectiveCamera& GetCamera(){ return m_Camera; }

		void SetEnvironment(const Environment& env);
		void SetSkybox(const std::shared_ptr<TextureCube>& skybox);

		Light GetLight() const { return m_Light; }

		float& GetSkyboxLod() { return m_SkyboxLod; }

		void AddEntity(Entity* entity);
		Entity* CreateEntity(const std::string& name = "");


	private:
		Light m_Light;

		std::string m_DebugName;
		std::vector<Entity*> m_Entities;
		ProjectiveCamera m_Camera;

		float m_LightMultiplier = 0.3f;

		Environment m_Environment;
		std::shared_ptr<TextureCube> m_SkyboxTexture;
		std::shared_ptr<MaterialInstance> m_SkyboxMaterial;

		float m_SkyboxLod = 1.0f;

		friend class SceneRenderer;
		friend class SceneHierarchyPanel;

	};

}
