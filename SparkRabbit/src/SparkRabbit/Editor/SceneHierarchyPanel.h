#pragma once
#include "SparkRabbit/Scene/Scene.h"

namespace SparkRabbit {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel(const std::shared_ptr<Scene>& scene);

		void SetContext(const std::shared_ptr<Scene>& scene);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity* entity, uint32_t& imguiEntityID, uint32_t& imguiMeshID);
		void DrawMeshNode(const std::shared_ptr<Mesh>& mesh, uint32_t& imguiMeshID);
		void MeshNodeHierarchy(const std::shared_ptr<Mesh>& mesh, aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);
	private:
		std::shared_ptr<Scene> m_Context;
		std::shared_ptr<Mesh> m_SelectionContext;
	};
}