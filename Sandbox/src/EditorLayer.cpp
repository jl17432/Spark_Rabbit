#include "EditorLayer.h"

#include "SparkRabbit/ImGui/ImGuizmo.h"
#include "SparkRabbit/Renderer/Renderer2D.h"
#include"SparkRabbit/Asset/AssetManager.h"
#include"SparkRabbit/Math/Math.h"

namespace SparkRabbit{
	static void ImGuiShowHelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	EditorLayer::EditorLayer()
		: m_SceneType(SceneType::Model), m_ProjectiveCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 1000.0f))
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		using namespace glm;

		m_CheckerboardTex = Texture2D::Create("assets/editor/Checkerboard.tga");
		m_playButtonTex = Texture2D::Create("assets/editor/playButton.png");
		m_stopButtonTex = Texture2D::Create("assets/editor/stopButton.png");

		//create Editor panel
		m_SceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>(m_EditorScene);
		m_AssetFilePanel = std::make_unique<FileSystemPanel>();
		m_defaultAssetsPanel = std::make_unique<defaultAssetsPanel>();

		m_SceneHierarchyPanel->SetSelectionChangedCallback(std::bind(&EditorLayer::SelectEntity, this, std::placeholders::_1));
		m_SceneHierarchyPanel->SetEntityDeletedCallback(std::bind(&EditorLayer::OnEntityDeleted, this, std::placeholders::_1));

		NewScene();
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnScenePlay()
	{
		m_SelectedSubmeshes.clear();

		m_SceneState = SceneState::Play;

		m_PlayScene = std::make_shared<Scene>();
		m_EditorScene->CopyTo(m_PlayScene);
		m_PlayScene->OnRuntimeStart();
		m_SceneHierarchyPanel->SetContext(m_PlayScene);
		m_Scene = m_PlayScene;
	}

	void EditorLayer::OnSceneStop()
	{
		m_PlayScene->OnRuntimeStop();
		m_SceneState = SceneState::Edit;

		//upload runtime scene
		m_PlayScene = nullptr;

		m_SelectedSubmeshes.clear();
		m_SceneHierarchyPanel->SetContext(m_EditorScene);
	}

	void EditorLayer::OnUpdate(TickTime ts)
	{
		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				if (m_AllowViewportCameraEvents)
					m_ProjectiveCamera.OnUpdate(ts);

				m_EditorScene->OnRenderEditor(ts, m_ProjectiveCamera);

				if (m_DrawOnTopBoundingBoxes)
				{
					Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
					auto viewProj = m_ProjectiveCamera.GetViewProjection();
					Renderer2D::BeginScene(viewProj, false);
					//Renderer::DrawBoundingBox(m_MeshEntity->GetMesh(), m_MeshEntity->Transform());
					Renderer2D::EndScene();
					Renderer::EndRenderPass();
				}

				if (m_SelectedSubmeshes.size() && false)
				{
					auto& selection = m_SelectedSubmeshes[0];

					if (selection.Mesh && selection.Entity.HasComponent<MeshComponent>())
					{
						Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
						auto viewProj = m_ProjectiveCamera.GetViewProjection();
						Renderer2D::BeginScene(viewProj, false);
						glm::vec4 color = (m_SelectionMode == SelectionMode::Entity) ? glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f } : glm::vec4{ 0.2f, 0.9f, 0.2f, 1.0f };
						Renderer::DrawBoundingBox(selection.Mesh->Box, selection.Entity.GetComponent<TransformComponent>().GetTransform() * selection.Mesh->Transform, color);
						Renderer2D::EndScene();
						Renderer::EndRenderPass();
					}
				}
				break;
			}

			case SceneState::Play:
			{
				if (m_AllowViewportCameraEvents)
					m_ProjectiveCamera.OnUpdate(ts);
				m_PlayScene->OnUpdate(ts);
				m_PlayScene->OnRenderRuntime(ts);
				break;
			}

			case SceneState::Pause:
			{
				if(m_AllowViewportCameraEvents)
					m_ProjectiveCamera.OnUpdate(ts);

				m_PlayScene->OnRenderRuntime(ts);
				break;
			}
		}
		
	}

	bool EditorLayer::Property(const std::string& name, bool& value)
	{
		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		bool result = ImGui::Checkbox(id.c_str(), &value);

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		return result;
	}

	void EditorLayer::Property(const std::string& name, float& value, float min, float max, EditorLayer::PropertyFlag flags)
	{
		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		ImGui::SliderFloat(id.c_str(), &value, min, max);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void EditorLayer::Property(const std::string& name, glm::vec2& value, EditorLayer::PropertyFlag flags)
	{
		Property(name, value, -1.0f, 1.0f, flags);
	}

	void EditorLayer::Property(const std::string& name, glm::vec2& value, float min, float max, EditorLayer::PropertyFlag flags)
	{
		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		ImGui::SliderFloat2(id.c_str(), glm::value_ptr(value), min, max);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void EditorLayer::Property(const std::string& name, glm::vec3& value, EditorLayer::PropertyFlag flags)
	{
		Property(name, value, -1.0f, 1.0f, flags);
	}

	void EditorLayer::Property(const std::string& name, glm::vec3& value, float min, float max, EditorLayer::PropertyFlag flags)
	{
		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		if ((int)flags & (int)PropertyFlag::ColorProperty)
			ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
		else
			ImGui::SliderFloat3(id.c_str(), glm::value_ptr(value), min, max);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void EditorLayer::Property(const std::string& name, glm::vec4& value, EditorLayer::PropertyFlag flags)
	{
		Property(name, value, -1.0f, 1.0f, flags);
	}

	void EditorLayer::Property(const std::string& name, glm::vec4& value, float min, float max, EditorLayer::PropertyFlag flags)
	{

		ImGui::Text(name.c_str());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string id = "##" + name;
		if ((int)flags & (int)PropertyFlag::ColorProperty)
			ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
		else
			ImGui::SliderFloat4(id.c_str(), glm::value_ptr(value), min, max);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	void EditorLayer::ShowBoundingBoxes(bool show, bool onTop)
	{
		SceneRenderer::GetOptions().ShowBoundingBoxes = show && !onTop;
		m_DrawOnTopBoundingBoxes = show && onTop;
	}

	void EditorLayer::NewScene()
	{
		m_EditorScene = std::make_shared<Scene>("Empty Scene", true);
		m_SceneHierarchyPanel->SetContext(m_EditorScene);

		m_ProjectiveCamera = ProjectiveCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 1000.0f));
	}

	void EditorLayer::SelectEntity(Entity entity)
	{
		SelectedSubmesh selection;
		if (entity.HasComponent<MeshComponent>())
		{
			auto& meshComp = entity.GetComponent<MeshComponent>();

			if (meshComp.Mesh)
			{
				selection.Mesh = &meshComp.Mesh->GetSubmeshes()[0];
			}
		}
		selection.Entity = entity;
		m_SelectedSubmeshes.clear();
		m_SelectedSubmeshes.push_back(selection);

		m_EditorScene->SetSelectedEntity(entity);

		m_Scene = m_EditorScene;
	}

	void EditorLayer::OnEntityDeleted(Entity entity)
	{
		if (m_SelectedSubmeshes.size() > 0 && m_SelectedSubmeshes[0].Entity == entity)
		{
			m_SelectedSubmeshes.clear();
			m_EditorScene->SetSelectedEntity({});
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool p_open = true;

		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		//if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
		//	window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}

		// Editor Panel ------------------------------------------------------------------------------
		ImGui::Begin("Environment");

		ImGui::SliderFloat("Skybox LOD", &m_EditorScene->GetSkyboxLod(), 0.0f, 11.0f);

		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();

		auto& light = m_EditorScene->GetLight();
		Property("Light Direction", light.Direction);
		Property("Light Radiance", light.Radiance, PropertyFlag::ColorProperty);
		Property("Light Multiplier", light.Multiplier, 0.0f, 5.0f);

		Property("Exposure", m_ProjectiveCamera.GetExposure(), 0.0f, 5.0f);

		Property("Radiance Prefiltering", m_RadiancePrefilter);
		Property("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f);

		if (Property("Show Bounding Boxes", m_UIShowBoundingBoxes))
			ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
		if (m_UIShowBoundingBoxes && Property("On Top", m_UIShowBoundingBoxesOnTop))
			ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);

		ImGui::Columns(1);

		ImGui::End();

		m_SceneHierarchyPanel->OnImGuiRender();
		m_AssetFilePanel->OnImGuiRender();
		m_defaultAssetsPanel->OnImGuiRender();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Materials");

		if (m_SelectedSubmeshes.size())
		{
			Entity selectedEntity = m_SelectedSubmeshes.front().Entity;
			if (selectedEntity.HasComponent<MeshComponent>())
			{
				std::shared_ptr<Mesh> mesh = selectedEntity.GetComponent<MeshComponent>().Mesh;
				if (mesh)
				{
					auto& materials = mesh->GetMaterials();
					static uint32_t selectedMaterialIndex = 0;
					for (uint32_t i = 0; i < materials.size(); i++)
					{
						auto& materialInstance = materials[i];

						ImGuiTreeNodeFlags node_flags = (selectedMaterialIndex == i ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_Leaf;
						bool opened = ImGui::TreeNodeEx((void*)(&materialInstance), node_flags, materialInstance->GetName().c_str());
						if (ImGui::IsItemClicked())
						{
							selectedMaterialIndex = i;
						}
						if (opened)
							ImGui::TreePop();

					}

					ImGui::Separator();

					// Selected material
					if (selectedMaterialIndex < materials.size())
					{
						auto& materialInstance = materials[selectedMaterialIndex];
						ImGui::Text("Shader: %s", materialInstance->GetShader()->GetName().c_str());
						// Textures ------------------------------------------------------------------------------
						{
							// Albedo
							if (ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
							{
								ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

								auto& albedoColor = materialInstance->Get<glm::vec3>("u_AlbedoColor");
								bool useAlbedoMap = materialInstance->Get<float>("u_AlbedoTexToggle");
								std::shared_ptr<Texture2D> albedoMap = materialInstance->TryGetResource<Texture2D>("u_AlbedoTexture");
								ImGui::Image(albedoMap ? (void*)albedoMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
								ImGui::PopStyleVar();
								if (ImGui::IsItemHovered())
								{
									if (albedoMap)
									{
										ImGui::BeginTooltip();
										ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
										ImGui::TextUnformatted(albedoMap->GetPath().c_str());
										ImGui::PopTextWrapPos();
										ImGui::Image((void*)albedoMap->GetRendererID(), ImVec2(384, 384));
										ImGui::EndTooltip();
									}
									if (ImGui::IsItemClicked())
									{
										std::string filename = Application::Get().OpenFile("");
										if (filename != "")
										{
											albedoMap = Texture2D::Create(filename, true/*m_AlbedoInput.SRGB*/);
											materialInstance->Set("u_AlbedoTexture", albedoMap);
										}
									}
								}
								ImGui::SameLine();
								ImGui::BeginGroup();
								if (ImGui::Checkbox("Use##AlbedoMap", &useAlbedoMap))
									materialInstance->Set<float>("u_AlbedoTexToggle", useAlbedoMap ? 1.0f : 0.0f);

								ImGui::EndGroup();
								ImGui::SameLine();
								ImGui::ColorEdit3("Color##Albedo", glm::value_ptr(albedoColor), ImGuiColorEditFlags_NoInputs);
							}
						}
						{
							// Normals
							if (ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
							{
								ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
								bool useNormalMap = materialInstance->Get<float>("u_NormalTexToggle");
								std::shared_ptr<Texture2D> normalMap = materialInstance->TryGetResource<Texture2D>("u_NormalTexture");
								ImGui::Image(normalMap ? (void*)normalMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
								ImGui::PopStyleVar();
								if (ImGui::IsItemHovered())
								{
									if (normalMap)
									{
										ImGui::BeginTooltip();
										ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
										ImGui::TextUnformatted(normalMap->GetPath().c_str());
										ImGui::PopTextWrapPos();
										ImGui::Image((void*)normalMap->GetRendererID(), ImVec2(384, 384));
										ImGui::EndTooltip();
									}
									if (ImGui::IsItemClicked())
									{
										std::string filename = Application::Get().OpenFile("");
										if (filename != "")
										{
											normalMap = Texture2D::Create(filename);
											materialInstance->Set("u_NormalTexture", normalMap);
										}
									}
								}
								ImGui::SameLine();
								if (ImGui::Checkbox("Use##NormalMap", &useNormalMap))
									materialInstance->Set<float>("u_NormalTexToggle", useNormalMap ? 1.0f : 0.0f);
							}
						}
						{
							// Metalness
							if (ImGui::CollapsingHeader("Metalness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
							{
								ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
								float& metalnessValue = materialInstance->Get<float>("u_Metalness");
								bool useMetalnessMap = materialInstance->Get<float>("u_MetalnessTexToggle");
								std::shared_ptr<Texture2D> metalnessMap = materialInstance->TryGetResource<Texture2D>("u_MetalnessTexture");
								ImGui::Image(metalnessMap ? (void*)metalnessMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
								ImGui::PopStyleVar();
								if (ImGui::IsItemHovered())
								{
									if (metalnessMap)
									{
										ImGui::BeginTooltip();
										ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
										ImGui::TextUnformatted(metalnessMap->GetPath().c_str());
										ImGui::PopTextWrapPos();
										ImGui::Image((void*)metalnessMap->GetRendererID(), ImVec2(384, 384));
										ImGui::EndTooltip();
									}
									if (ImGui::IsItemClicked())
									{
										std::string filename = Application::Get().OpenFile("");
										if (filename != "")
										{
											metalnessMap = Texture2D::Create(filename);
											materialInstance->Set("u_MetalnessTexture", metalnessMap);
										}
									}
								}
								ImGui::SameLine();
								if (ImGui::Checkbox("Use##MetalnessMap", &useMetalnessMap))
									materialInstance->Set<float>("u_MetalnessTexToggle", useMetalnessMap ? 1.0f : 0.0f);
								ImGui::SameLine();
								ImGui::SliderFloat("Value##MetalnessInput", &metalnessValue, 0.0f, 1.0f);
							}
						}
						{
							// Roughness
							if (ImGui::CollapsingHeader("Roughness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
							{
								ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
								float& roughnessValue = materialInstance->Get<float>("u_Roughness");
								bool useRoughnessMap = materialInstance->Get<float>("u_RoughnessTexToggle");
								std::shared_ptr<Texture2D> roughnessMap = materialInstance->TryGetResource<Texture2D>("u_RoughnessTexture");
								ImGui::Image(roughnessMap ? (void*)roughnessMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
								ImGui::PopStyleVar();
								if (ImGui::IsItemHovered())
								{
									if (roughnessMap)
									{
										ImGui::BeginTooltip();
										ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
										ImGui::TextUnformatted(roughnessMap->GetPath().c_str());
										ImGui::PopTextWrapPos();
										ImGui::Image((void*)roughnessMap->GetRendererID(), ImVec2(384, 384));
										ImGui::EndTooltip();
									}
									if (ImGui::IsItemClicked())
									{
										std::string filename = Application::Get().OpenFile("");
										if (filename != "")
										{
											roughnessMap = Texture2D::Create(filename);
											materialInstance->Set("u_RoughnessTexture", roughnessMap);
										}
									}
								}
								ImGui::SameLine();
								if (ImGui::Checkbox("Use##RoughnessMap", &useRoughnessMap))
									materialInstance->Set<float>("u_RoughnessTexToggle", useRoughnessMap ? 1.0f : 0.0f);
								ImGui::SameLine();
								ImGui::SliderFloat("Value##RoughnessInput", &roughnessValue, 0.0f, 1.0f);
							}
						}
					}
				}
			}
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		auto viewportSize = ImGui::GetContentRegionAvail();
		float buttonWidth = 32.0f;
		float buttonPosX = (viewportSize.x - buttonWidth) / 2.0f;

		if (m_SceneState == SceneState::Edit)
		{
			ImGui::SetCursorPosX(buttonPosX);
			if (ImGui::ImageButton((ImTextureID)(m_playButtonTex->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1.f, 1.f, 1.f, 1.f)))
			{
				OnScenePlay();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Play");
		}
		else if (m_SceneState == SceneState::Play)
		{
			ImGui::SetCursorPosX(buttonPosX);
			if (ImGui::ImageButton((ImTextureID)(m_stopButtonTex->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.f)))
			{
				OnSceneStop();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Stop");
		}

		viewportSize = ImGui::GetContentRegionAvail();
		auto viewportOffset = ImGui::GetCursorPos();
		SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		m_EditorScene->SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		if(m_PlayScene)
			m_PlayScene->SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		m_ProjectiveCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 1000.0f));
		m_ProjectiveCamera.SetViewport((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		ImGui::Image((void*)SceneRenderer::GetFinalColorBufferRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

		static int counter = 0;
		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = ImGui::GetWindowPos();
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;

		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
		m_ViewportBounds[0] = { minBound.x, minBound.y };
		m_ViewportBounds[1] = { maxBound.x, maxBound.y };
		m_AllowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBound, maxBound);

		// Gizmos
		if (m_GizmoType != -1 && m_SelectedSubmeshes.size())
		{
			auto& selection = m_SelectedSubmeshes[0];

			float rw = (float)ImGui::GetWindowWidth();
			float rh = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);

			bool snap = Input::IsKeyPressed(SR_KEY_LEFT_CONTROL);

			TransformComponent& entityTransform = selection.Entity.Transform();
			glm::mat4 transform = m_Scene->GetTransformRelativeToParent(selection.Entity);
			float snapValue = GetSnapValue();
			float snapValues[3] = { snapValue, snapValue, snapValue };

			if (m_SelectionMode == SelectionMode::Entity)
			{
				ImGuizmo::Manipulate(glm::value_ptr(m_ProjectiveCamera.GetView()),
					glm::value_ptr(m_ProjectiveCamera.GetProjectionMatrix()),
					(ImGuizmo::OPERATION)m_GizmoType,
					ImGuizmo::LOCAL,
					glm::value_ptr(transform),
					nullptr,
					snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);

					Entity parent = m_Scene->FindEntityByUUID(selection.Entity.GetParentUUID());
					if (parent)
					{
						glm::vec3 parentTranslation, parentRotation, parentScale;
						Math::DecomposeTransform(m_Scene->GetTransformRelativeToParent(parent), parentTranslation, parentRotation, parentScale);

						glm::vec3 deltaRotation = (rotation - parentRotation) - entityTransform.Rotation;
						entityTransform.Translation = translation - parentTranslation;
						entityTransform.Rotation += deltaRotation;
						entityTransform.Scale = scale;
					}
					else
					{
						glm::vec3 deltaRotation = rotation - entityTransform.Rotation;
						entityTransform.Translation = translation;
						entityTransform.Rotation += deltaRotation;
						entityTransform.Scale = scale;
					}
				}
			}
			else
			{
				glm::mat4 transformBase = transform * selection.Mesh->Transform;
				ImGuizmo::Manipulate(glm::value_ptr(m_ProjectiveCamera.GetView()),
					glm::value_ptr(m_ProjectiveCamera.GetProjectionMatrix()),
					(ImGuizmo::OPERATION)m_GizmoType,
					ImGuizmo::LOCAL,
					glm::value_ptr(transformBase),
					nullptr,
					snap ? snapValues : nullptr);

				selection.Mesh->Transform = glm::inverse(transform) * transformBase;
			}
		}


		//Drag and drop asset file from content browser to Scene
		if (ImGui::BeginDragDropTarget())
		{
			auto data = ImGui::AcceptDragDropPayload("Asset Load");
			if (data)
			{
				std::string path(reinterpret_cast<const char*>(data->Data));
				std::shared_ptr<Asset> asset = AssetManager::GetAsset<Asset>(path);
				if (asset->Type == AssetType::Mesh)
				{
					Entity entity = m_EditorScene->CreateEntity(asset->FileName);
					entity.AddComponent<MeshComponent>(std::dynamic_pointer_cast<Mesh>(asset));
					SelectEntity(entity);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (m_AllowViewportCameraEvents)
			m_ProjectiveCamera.OnEvent(e);

		m_EditorScene->OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SR_BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<MouseButtonPressedEvent>(SR_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case SR_KEY_Q:
			m_GizmoType = -1;
			break;
		case SR_KEY_W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case SR_KEY_E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case SR_KEY_R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		case SR_KEY_G:
			// Toggle grid
			if (Input::IsKeyPressed(SR_KEY_LEFT_CONTROL))
				SceneRenderer::GetOptions().ShowGrid = !SceneRenderer::GetOptions().ShowGrid;
			break;
		case SR_KEY_B:
			// Toggle bounding boxes 
			if (Input::IsKeyPressed(SR_KEY_LEFT_CONTROL))
			{
				m_UIShowBoundingBoxes = !m_UIShowBoundingBoxes;
				ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
			}
			break;
		}
		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		auto [mx, my] = Input::GetMousePos();
		if (e.GetMouseButton() == SR_MOUSE_BUTTON_LEFT && !Input::IsKeyPressed(SR_KEY_LEFT_ALT) && !ImGuizmo::IsOver())
		{
			auto [mouseX, mouseY] = GetMouseViewportSpace();
			if (mouseX > -1.0f && mouseX < 1.0f && mouseY > -1.0f && mouseY < 1.0f)
			{
				auto [origin, direction] = CastRay(mouseX, mouseY);

				m_SelectedSubmeshes.clear();
				m_EditorScene->SetSelectedEntity({});
				auto meshEntities = m_EditorScene->GetAllEntitiesWith<MeshComponent>();
				for (auto e : meshEntities)
				{
					Entity entity = { e, m_EditorScene.get()};
					auto mesh = entity.GetComponent<MeshComponent>().Mesh;
					if (!mesh)
						continue;

					auto& submeshes = mesh->GetSubmeshes();
					constexpr float lastT = std::numeric_limits<float>::max();
					for (uint32_t i = 0; i < submeshes.size(); i++)
					{
						auto& submesh = submeshes[i];
						glm::mat4 transform = m_EditorScene->GetTransformRelativeToParent(entity);
						Ray ray = {
							glm::inverse(transform * submesh.Transform) * glm::vec4(origin, 1.0f),
							glm::inverse(glm::mat3(transform) * glm::mat3(submesh.Transform)) * direction
						};

						float t;
						bool intersects = ray.IntersectsBoundingBox(submesh.Box, t);
						if (intersects)
						{
							const auto& triangleCache = mesh->GetTriangleCache(i);
							for (const auto& triangle : triangleCache)
							{
								if (ray.IntersectsTriangle(triangle.V0.Position, triangle.V1.Position, triangle.V2.Position, t))
								{
									SPARK_WARN("INTERSECTION: {0}, t={1}", submesh.NodeName, t);
									m_SelectedSubmeshes.push_back({ entity, &submesh, t });
									break;
								}
							}
						}
					}
				}
				std::sort(m_SelectedSubmeshes.begin(), m_SelectedSubmeshes.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });

				if (m_SelectedSubmeshes.size())
					OnSelected(m_SelectedSubmeshes[0]);

			}
		}
		return false;
	}

	std::pair<float, float> EditorLayer::GetMouseViewportSpace()
	{
		auto [mx, my] = ImGui::GetMousePos(); // Input::GetMousePosition();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		auto viewportWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
		auto viewportHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;

		return { (mx / viewportWidth) * 2.0f - 1.0f, ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f };
	}

	std::pair<glm::vec3, glm::vec3> EditorLayer::CastRay(float mx, float my)
	{
		glm::vec4 mouseClipPos = { mx, my, -1.0f, 1.0f };

		auto inverseProj = glm::inverse(m_ProjectiveCamera.GetProjection());
		auto inverseView = glm::inverse(glm::mat3(m_ProjectiveCamera.GetView()));

		glm::vec4 ray = inverseProj * mouseClipPos;
		glm::vec3 rayPos = m_ProjectiveCamera.GetPosition();
		glm::vec3 rayDir = inverseView * glm::vec3(ray);

		return { rayPos, rayDir };
	}

	float EditorLayer::GetSnapValue()
	{
		switch (m_GizmoType)
		{
		case  ImGuizmo::OPERATION::TRANSLATE: return 0.5f;
		case  ImGuizmo::OPERATION::ROTATE: return 45.0f;
		case  ImGuizmo::OPERATION::SCALE: return 0.5f;
		}
		return 0.0f;
	}

	void EditorLayer::OnSelected(const SelectedSubmesh& m_SelectedSubmeshes)
	{
		m_SceneHierarchyPanel->SetSelected(m_SelectedSubmeshes.Entity);
		m_EditorScene->SetSelectedEntity(m_SelectedSubmeshes.Entity);
	}

}
