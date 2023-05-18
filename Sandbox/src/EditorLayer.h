#pragma once
#include "SparkAPI.h"

#include <string>
#include "SparkRabbit/Editor/SceneHierarchyPanel.h"
#include "SparkRabbit/Editor/FileSystemPanel.h"
#include "SparkRabbit/Editor/defaultAssetsPanel.h"
#include "SparkRabbit/Renderer/Renderer2D.h"
#include "SparkRabbit/Asset/AssetManager.h"
#include "SparkRabbit/Math/Math.h"

#include "SparkRabbit/ImGui/ImGuiLayer.h"
#include "imgui/imgui_internal.h"
#include "SparkRabbit/ImGui/ImGuizmo.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


namespace SparkRabbit {
	class EditorLayer : public Layer
	{
	public:
		enum class PropertyFlag
		{
			None = 0, ColorProperty = 1
		};
	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(TickTime ts) override;

		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);


		bool Property(const std::string& name, bool& value);
		void Property(const std::string& name, float& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
		void Property(const std::string& name, glm::vec2& value, PropertyFlag flags);
		void Property(const std::string& name, glm::vec2& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
		void Property(const std::string& name, glm::vec3& value, PropertyFlag flags);
		void Property(const std::string& name, glm::vec3& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
		void Property(const std::string& name, glm::vec4& value, PropertyFlag flags);
		void Property(const std::string& name, glm::vec4& value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);

		void ShowBoundingBoxes(bool show);

		void NewScene();

		void SelectEntity(Entity entity);
		void OnEntityDeleted(Entity entity);

		void OnScenePlay();
		void OnSceneStop();
	private:
		std::pair<float, float> GetMouseViewportSpace();
		std::pair<glm::vec3, glm::vec3> CastRay(float mx, float my);

		float GetSnapValue();
	private:
		std::unique_ptr<SceneHierarchyPanel> m_SceneHierarchyPanel;
		std::unique_ptr<FileSystemPanel> m_AssetFilePanel;
		std::unique_ptr<defaultAssetsPanel> m_defaultAssetsPanel;

		std::shared_ptr<Scene> m_Scene;
		std::shared_ptr<Scene> m_EditorScene;
		std::shared_ptr<Scene> m_PlayScene;

		ProjectiveCamera m_EditorCamera;

		float m_GridScale = 16.025f, m_GridSize = 0.025f;

		struct AlbedoInput
		{
			glm::vec3 Color = { 0.972f, 0.96f, 0.915f };
			std::shared_ptr<Texture2D> TextureMap;
			bool SRGB = true;
			bool UseTexture = false;
		};
		AlbedoInput m_AlbedoInput;

		struct NormalInput
		{
			std::shared_ptr<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		NormalInput m_NormalInput;

		struct MetalnessInput
		{
			float Value = 1.0f;
			std::shared_ptr<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		MetalnessInput m_MetalnessInput;

		struct RoughnessInput
		{
			float Value = 0.2f;
			std::shared_ptr<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		RoughnessInput m_RoughnessInput;

		// PBR parameters
		bool m_RadiancePrefilter = false;

		float m_EnvMapRotation = 0.0f;

		// Editor resources
		std::shared_ptr<Texture2D> m_CheckerboardTex;
		std::shared_ptr<Texture2D> m_playButtonTex;
		std::shared_ptr<Texture2D> m_stopButtonTex;

		glm::vec2 m_ViewportBounds[2];
		int m_GizmoType = -1; // no gizmo
		bool m_AllowViewportCameraEvents = false;
		bool m_DrawOnTopBoundingBoxes = false;
		bool m_UIShowBoundingBoxes = false;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;

		struct SelectedSubmesh
		{
			SparkRabbit::Entity Entity;
			Submesh* Mesh = nullptr;
			float Distance = 0.0f;
		};
		void OnSelected(const SelectedSubmesh& m_SelectedSubmeshes);
		std::vector<SelectedSubmesh> m_SelectedSubmeshes;

		enum class SelectionMode
		{
			None = 0, Entity = 1, SubMesh = 2
		};
		SelectionMode m_SelectionMode = SelectionMode::Entity;

		glm::mat4* m_CurrentlySelectedTransform = nullptr;
	};
}


