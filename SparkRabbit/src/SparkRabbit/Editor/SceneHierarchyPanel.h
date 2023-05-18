#pragma once
#include "SparkRabbit/Scene/Scene.h"
#include "SparkRabbit/Scene/Entity.h"
#include "SparkRabbit/Renderer/Mesh.h"
#include "SparkRabbit/Math/Math.h"
#include "SparkRabbit/Scene/Components.h"

#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#include "SparkRabbit/ImGui/ImGui.h"

namespace SparkRabbit {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel(const std::shared_ptr<Scene>& scene);

		void SetContext(const std::shared_ptr<Scene>& scene);
		void SetSelected(Entity entity);
		void SetSelectionChangedCallback(const std::function<void(Entity)>& func) { m_SelectionChangedCallback = func; }
		void SetEntityDeletedCallback(const std::function<void(Entity)>& func) { m_EntityDeletedCallback = func; }

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawMeshNode(const std::shared_ptr<Mesh>& mesh, uint32_t& imguiMeshID);
		void MeshNodeHierarchy(const std::shared_ptr<Mesh>& mesh, aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);
		void DrawComponents(Entity entity);
	private:
		std::shared_ptr<Scene> m_Context;
		Entity m_SelectionContext;

		std::function<void(Entity)> m_SelectionChangedCallback, m_EntityDeletedCallback;
	};
}