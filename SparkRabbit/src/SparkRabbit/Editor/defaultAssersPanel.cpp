#include "PrecompileH.h"
#include "defaultAssetsPanel.h"
#include "SparkRabbit/ImGui/ImGui.h"

namespace SparkRabbit {
	defaultAssetsPanel::defaultAssetsPanel()
	{
		m_CubeImage = Texture2D::Create("Resources/Icons/asset.png");
	}
	void defaultAssetsPanel::OnImGuiRender()
	{
		std::string cubePath = "assets/meshes/cube.fbx";
		std::string spherePath = "assets/meshes/sphere.fbx";
		std::string capsulePath = "assets/meshes/Capsule.fbx";
		std::string cylinderPath = "assets/meshes/Cylindrical.fbx";
		std::string torusPath = "assets/meshes/torus.fbx";
		std::string chocolatePath = "assets/meshes/chocolate.fbx";
		std::string gunPath = "assets/meshes/gun.fbx";
		std::string planePath = "assets/meshes/plane.fbx";


		ImGui::Begin("Objects");
		{
			ImGui::BeginChild("##objects_window");
			DrawObject("Cube", cubePath);
			DrawObject("Plane", planePath);
			DrawObject("Sphere", spherePath);
			DrawObject("Capsule", capsulePath);
			DrawObject("Cylinder", cylinderPath);
			DrawObject("Torus", torusPath);
			DrawObject("Chocolate", chocolatePath);
			DrawObject("Gun", gunPath);
			ImGui::EndChild();
		}

		ImGui::End();
	}
	void defaultAssetsPanel::DrawObject(const char* label, std::string path)
	{
		ImGui::Image((ImTextureID)m_CubeImage->GetRendererID(), ImVec2(30, 30));
		ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Selectable(label);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::Image((ImTextureID)m_CubeImage->GetRendererID(), ImVec2(20, 20));
			ImGui::SameLine();

			ImGui::Text(label);

			ImGui::SetDragDropPayload("Asset Load", path.c_str(), path.size() + 1);
			ImGui::EndDragDropSource();
		}
	}
}