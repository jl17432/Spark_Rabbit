#include "PrecompileH.h"
#include "SceneHierarchyPanel.h"
#include "SparkRabbit/Renderer/Mesh.h"
#include"SparkRabbit/Math/Math.h"
#include"SparkRabbit/Renderer/Mesh.h"
#include"SparkRabbit/Scene/Components.h"


#include"imgui.h"
#include <imgui/imgui_internal.h>
#include"SparkRabbit/ImGui/ImGui.h"

#include <assimp/scene.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace SparkRabbit {

	glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix);

	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& context)
		: m_Context(context)
	{
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& scene)
	{
		m_Context = scene;
		m_SelectionContext = {};
		if (m_SelectionContext && false)
		{
			// Try and find same entity in new scene
			auto& entityMap = m_Context->GetEntityMap();
			UUID selectedEntityID = m_SelectionContext.GetUUID();
			if (entityMap.find(selectedEntityID) != entityMap.end())
				m_SelectionContext = entityMap.at(selectedEntityID);
		}
	}

	void SceneHierarchyPanel::SetSelected(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		ImRect windowRect = { ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax() };

		if (m_Context)
		{
			uint32_t entityCount = 0, meshCount = 0;
			m_Context->m_Registry.each([&](auto entity)
				{
					Entity e(entity, m_Context.get());
					if (e.HasComponent<IDComponent>() && e.GetParentUUID() == 0)
						DrawEntityNode(e);
				});

			if (ImGui::BeginDragDropTargetCustom(windowRect, ImGui::GetCurrentWindow()->ID))
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("scene_entity_hierarchy", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

				if (payload)
				{
					UUID droppedHandle = *((UUID*)payload->Data);
					Entity e = m_Context->FindEntityByUUID(droppedHandle);
					Entity previousParent = m_Context->FindEntityByUUID(e.GetParentUUID());

					if (previousParent)
					{
						auto& children = previousParent.Children();
						children.erase(std::remove(children.begin(), children.end(), droppedHandle), children.end());

						glm::mat4 parentTransform = m_Context->GetTransformRelativeToParent(previousParent);
						glm::vec3 parentTranslation, parentRotation, parentScale;
						Math::DecomposeTransform(parentTransform, parentTranslation, parentRotation, parentScale);

						e.Transform().Translation = e.Transform().Translation + parentTranslation;
					}

					e.SetParentUUID(0);
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::MenuItem("Empty Entity"))
					{
						auto newEntity = m_Context->CreateEntity("Empty Entity");
						SetSelected(newEntity);
					}
					if (ImGui::MenuItem("Mesh"))
					{
						auto newEntity = m_Context->CreateEntity("Mesh");
						newEntity.AddComponent<MeshComponent>();
						SetSelected(newEntity);
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Directional Light"))
					{
						auto newEntity = m_Context->CreateEntity("Directional Light");
						newEntity.AddComponent<DirectionalLightComponent>();
						newEntity.GetComponent<TransformComponent>().Rotation = glm::radians(glm::vec3{ 80.0f, 10.0f, 0.0f });
						SetSelected(newEntity);
					}
					ImGui::EndMenu();
				}
				ImGui::EndPopup();
			}

			ImGui::End();

			ImGui::Begin("Properties");

			if (m_SelectionContext)
				DrawComponents(m_SelectionContext);
		}
		ImGui::End();


#if TODO
		ImGui::Begin("Mesh Debug");
		if (ImGui::CollapsingHeader(mesh->m_FilePath.c_str()))
		{
			if (mesh->m_IsAnimated)
			{
				if (ImGui::CollapsingHeader("Animation"))
				{
					if (ImGui::Button(mesh->m_AnimationPlaying ? "Pause" : "Play"))
						mesh->m_AnimationPlaying = !mesh->m_AnimationPlaying;

					ImGui::SliderFloat("##AnimationTime", &mesh->m_AnimationTime, 0.0f, (float)mesh->m_Scene->mAnimations[0]->mDuration);
					ImGui::DragFloat("Time Scale", &mesh->m_TimeMultiplier, 0.05f, 0.0f, 10.0f);
				}
			}
		}
		ImGui::End();
#endif
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		const char* name = "Unnamed Entity";
		if (entity.HasComponent<TagComponent>())
			name = entity.GetComponent<TagComponent>().Tag.c_str();

		ImGuiTreeNodeFlags flags = (entity == m_SelectionContext ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		if (entity.Children().empty())
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, name);
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
			if (m_SelectionChangedCallback)
				m_SelectionChangedCallback(m_SelectionContext);
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			UUID entityId = entity.GetUUID();
			ImGui::Text(entity.GetComponent<TagComponent>().Tag.c_str());
			ImGui::SetDragDropPayload("scene_entity_hierarchy", &entityId, sizeof(UUID));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("scene_entity_hierarchy", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

			if (payload)
			{
				UUID droppedHandle = *((UUID*)payload->Data);
				Entity e = m_Context->FindEntityByUUID(droppedHandle);

				if (!entity.IsDescendantOf(e))
				{
					// Remove from previous parent
					Entity previousParent = m_Context->FindEntityByUUID(e.GetParentUUID());
					if (previousParent)
					{
						auto& parentChildren = previousParent.Children();
						parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), droppedHandle), parentChildren.end());
					}

					glm::mat4 parentTransform = m_Context->GetTransformRelativeToParent(entity);
					glm::vec3 parentTranslation, parentRotation, parentScale;
					Math::DecomposeTransform(parentTransform, parentTranslation, parentRotation, parentScale);

					e.Transform().Translation = e.Transform().Translation - parentTranslation;
					e.SetParentUUID(entity.GetUUID());
					entity.Children().push_back(droppedHandle);
				}

			}

			ImGui::EndDragDropTarget();
		}

		if (opened)
		{
			for (auto child : entity.Children())
			{
				Entity e = m_Context->FindEntityByUUID(child);
				if (e)
					DrawEntityNode(e);
			}

			ImGui::TreePop();
		}

		// Defer deletion until end of node UI
		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (entity == m_SelectionContext)
				m_SelectionContext = {};

			m_EntityDeletedCallback(entity);
		}
	}

	void SceneHierarchyPanel::DrawMeshNode(const std::shared_ptr<Mesh>& mesh, uint32_t& imguiMeshID)
	{
		static char imguiName[128];
		memset(imguiName, 0, 128);
		sprintf(imguiName, "Mesh##%d", imguiMeshID++);

		// Mesh Hierarchy
		if (ImGui::TreeNode(imguiName))
		{
			auto rootNode = mesh->m_Scene->mRootNode;
			MeshNodeHierarchy(mesh, rootNode);
			ImGui::TreePop();
		}
	}

	static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4& transform)
	{
		glm::vec3 scale, translation, skew;
		glm::vec4 perspective;
		glm::quat orientation;
		glm::decompose(transform, scale, orientation, translation, skew, perspective);

		return { translation, orientation, scale };
	}

	void SceneHierarchyPanel::MeshNodeHierarchy(const std::shared_ptr<Mesh>& mesh, aiNode* node, const glm::mat4& parentTransform, uint32_t level)
	{
		glm::mat4 localTransform = Mat4FromAssimpMat4(node->mTransformation);
		glm::mat4 transform = parentTransform * localTransform;

		if (ImGui::TreeNode(node->mName.C_Str()))
		{
			{
				auto [translation, rotation, scale] = GetTransformDecomposition(transform);
				ImGui::Text("World Transform");
				ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
				ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
			}
			{
				auto [translation, rotation, scale] = GetTransformDecomposition(localTransform);
				ImGui::Text("Local Transform");
				ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
				ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
			}

			for (uint32_t i = 0; i < node->mNumChildren; i++)
				MeshNodeHierarchy(mesh, node->mChildren[i], transform, level + 1);

			ImGui::TreePop();
		}

	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{

			ImGui::PushID((void*)typeid(T).hash_code());
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx("##dummy_id", treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();

			ImGui::PopID();
		}
	}

	static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		bool modified = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			modified = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return modified;
	}
	std::vector<CollisionShape> getCollisionShapes()
	{
		return { CollisionShape::None, CollisionShape::Box, CollisionShape::Sphere, CollisionShape::ConvexHull };
	}

	std::string getCollisionShapeName(CollisionShape shape)
	{
		switch (shape)
		{
		case CollisionShape::None: return "None";
		case CollisionShape::Box: return "Box";
		case CollisionShape::Sphere: return "Sphere";
		case CollisionShape::ConvexHull: return "ConvexHull";
		default: return "Unknown";
		}
	}
	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		ImGui::AlignTextToFramePadding();

		auto id = entity.GetComponent<IDComponent>().ID;

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, 256);
			memcpy(buffer, tag.c_str(), tag.length());
			ImGui::PushItemWidth(contentRegionAvailable.x * 0.5f);
			if (ImGui::InputText("##Tag", buffer, 256))
			{
				tag = std::string(buffer);
			}
			ImGui::PopItemWidth();
		}

		// ID
		ImGui::SameLine();
		ImGui::TextDisabled("%llx", id);
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 textSize = ImGui::CalcTextSize("Add Component");
		ImGui::SameLine(contentRegionAvailable.x - (textSize.x + GImGui->Style.FramePadding.y));
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponentPanel");

		if (ImGui::BeginPopup("AddComponentPanel"))
		{
			if (!m_SelectionContext.HasComponent<CameraComponent>())
			{
				if (ImGui::Button("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<MeshComponent>())
			{
				if (ImGui::Button("Mesh"))
				{
					MeshComponent& component = m_SelectionContext.AddComponent<MeshComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<DirectionalLightComponent>())
			{
				if (ImGui::Button("Directional Light"))
				{
					m_SelectionContext.AddComponent<DirectionalLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<SkyLightComponent>())
			{
				if (ImGui::Button("Sky Light"))
				{
					m_SelectionContext.AddComponent<SkyLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::Button("Sprite Renderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<RigidBody2DComponent>())
			{
				if (ImGui::Button("RigidBody2D"))
				{
					m_SelectionContext.AddComponent<RigidBody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::Button("BoxCollider2D"))
				{
					m_SelectionContext.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<CircleCollider2DComponent>())
			{
				if (ImGui::Button("CircleCollider2D"))
				{
					m_SelectionContext.AddComponent<CircleCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<Rigidbody3DComponent>())
			{
				if (ImGui::Button("Rigidbody 3D"))
				{
					m_SelectionContext.AddComponent<Rigidbody3DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
			{
				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<MeshComponent>("Mesh", entity, [](MeshComponent& mc)
			{
				UI::BeginPropertyGrid();
				UI::PropertyAssetReference("Mesh", mc.Mesh, AssetType::Mesh);
				UI::EndPropertyGrid();
			});

		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cc)
			{
				// Projection Type
				const char* projTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProj = projTypeStrings[(int)cc.Camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProj))
				{
					for (int type = 0; type < 2; type++)
					{
						bool is_selected = (currentProj == projTypeStrings[type]);
						if (ImGui::Selectable(projTypeStrings[type], is_selected))
						{
							currentProj = projTypeStrings[type];
							cc.Camera.SetProjectionType((SceneCamera::ProjectionType)type);
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				UI::BeginPropertyGrid();
				// Perspective parameters
				if (cc.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float verticalFOV = cc.Camera.GetPerspectiveVerticalFOV();
					if (UI::Property("Vertical FOV", verticalFOV))
						cc.Camera.SetPerspectiveVerticalFOV(verticalFOV);

					float nearClip = cc.Camera.GetPerspectiveNearClip();
					if (UI::Property("Near Clip", nearClip))
						cc.Camera.SetPerspectiveNearClip(nearClip);
					ImGui::SameLine();
					float farClip = cc.Camera.GetPerspectiveFarClip();
					if (UI::Property("Far Clip", farClip))
						cc.Camera.SetPerspectiveFarClip(farClip);
				}

				// Orthographic parameters
				else if (cc.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = cc.Camera.GetOrthographicSize();
					if (UI::Property("Size", orthoSize))
						cc.Camera.SetOrthographicSize(orthoSize);

					float nearClip = cc.Camera.GetOrthographicNearClip();
					if (UI::Property("Near Clip", nearClip))
						cc.Camera.SetOrthographicNearClip(nearClip);
					ImGui::SameLine();
					float farClip = cc.Camera.GetOrthographicFarClip();
					if (UI::Property("Far Clip", farClip))
						cc.Camera.SetOrthographicFarClip(farClip);
				}

				UI::EndPropertyGrid();
			});

		DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](DirectionalLightComponent& dlc)
			{
				UI::BeginPropertyGrid();
				UI::PropertyColor("Radiance", dlc.Radiance);
				UI::Property("Intensity", dlc.Intensity);
				UI::Property("Cast Shadows", dlc.CastShadows);
				UI::Property("Soft Shadows", dlc.SoftShadows);
				UI::Property("Source Size", dlc.LightSize);
				UI::EndPropertyGrid();
			});

		DrawComponent<RigidBody2DComponent>("RigidBody2D", entity, [](RigidBody2DComponent& rb2d)
			{
				// Rigid body type
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentbodyTypeStrings = bodyTypeStrings[(int)rb2d.Type];
				if (ImGui::BeginCombo("Body Tye", currentbodyTypeStrings))
				{
					for (int type = 0; type < 2; type++)
					{
						bool is_selected = (currentbodyTypeStrings == bodyTypeStrings[type]);
						if (ImGui::Selectable(bodyTypeStrings[type], is_selected))
						{
							currentbodyTypeStrings = bodyTypeStrings[type];
							rb2d.Type = (RigidBody2DComponent::BodyType)type;
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::Checkbox("Fixed Rotation", &rb2d.FixedRotation);
			});

		DrawComponent<BoxCollider2DComponent>("BoxCollider2D", entity, [](BoxCollider2DComponent& bc2d)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(bc2d.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(bc2d.Size));
				ImGui::DragFloat("Density", &bc2d.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &bc2d.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &bc2d.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &bc2d.RestitutionThreshold, 0.01f, 0.0f);
			});
		DrawComponent<CircleCollider2DComponent>("CircleCollider2D", entity, [](CircleCollider2DComponent& cc2d)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(cc2d.Offset));
				ImGui::DragFloat("Radius", &cc2d.Radius);
				ImGui::DragFloat("Density", &cc2d.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &cc2d.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &cc2d.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &cc2d.RestitutionThreshold, 0.01f, 0.0f);
			});
		DrawComponent<Rigidbody3DComponent>("Rigidbody 3D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 150.0f);
				ImGui::Text("Body Type");
				ImGui::NextColumn();
				if (ImGui::BeginCombo("##Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 3; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody3DComponent::BodyType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				ImGui::EndColumns();

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 150.0f);
				ImGui::Text("Collision Shape");
				/*ImGui::NextColumn();
				constexpr auto collisionShapes = magic_enum::enum_values<CollisionShape>();
				if (ImGui::BeginCombo("##Collision Shape", magic_enum::enum_name(component.Shape).data()))
				{
					for (auto& shape : collisionShapes)
					{
						bool isSelected = component.Shape == shape;
						if (ImGui::Selectable(magic_enum::enum_name(shape).data(), isSelected))
						{
							component.Shape = shape;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				ImGui::EndColumns();*/
				ImGui::NextColumn();
				if (ImGui::BeginCombo("##Collision Shape", getCollisionShapeName(component.Shape).c_str()))
				{
					auto shapes = getCollisionShapes();
					for (size_t i = 0; i < shapes.size(); ++i)
					{
						bool isSelected = component.Shape == shapes[i];
						if (ImGui::Selectable(getCollisionShapeName(shapes[i]).c_str(), isSelected))
						{
							component.Shape = shapes[i];
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				ImGui::EndColumns();
				if (component.Shape != CollisionShape::None)
				{
					const auto& floatValueUI = [](const char* name, float& value) {
						ImGui::Columns(2, nullptr, false);
						ImGui::SetColumnWidth(0, 150.0f);
						ImGui::Text(name);
						ImGui::NextColumn();
						std::string label = std::string("##") + std::string(name);
						ImGui::SliderFloat(label.c_str(), &value, 0.0f, 1.0f, "%.2f");
						ImGui::EndColumns();
					};

					floatValueUI("linearDamping", component.linearDamping);
					floatValueUI("angularDamping", component.angularDamping);
					floatValueUI("restitution", component.restitution);
					floatValueUI("friction", component.friction);
				}
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 150.0f);
				ImGui::Text("mass");
				ImGui::NextColumn();
				ImGui::SliderFloat("##masas", &component.mass, 0.0f, 10.0f, "%.2f");
				ImGui::EndColumns();

			});
	}
}