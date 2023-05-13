#include "PrecompileH.h"
#include "FileSystemPanel.h"

#include <imgui/imgui.h>
#include"SparkRabbit/Asset/AssetManager.h"

namespace SparkRabbit {
	static const std::filesystem::path m_AssetDirectory = "assets";

	FileSystemPanel::FileSystemPanel() : m_CurrentDirectory(m_AssetDirectory)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/folder.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/file.png");
		m_BackIcon = Texture2D::Create("Resources/Icons/back.png");
	}

	void FileSystemPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(m_AssetDirectory))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			if (ImGui::ImageButton((ImTextureID)(m_BackIcon->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1.f, 1.f, 1.f, 1.f)))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
			ImGui::PopStyleColor();
		}

		static float padding = 10.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();
			std::string extensionString = path.extension().string();
			if (extensionString == ".meta")
			{
				continue;
			}

			ImGui::PushID(filenameString.c_str());
			std::shared_ptr<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1.f, 1.f, 1.f, 1.f));

			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				std::string pathString = relativePath.string();
				std::replace(pathString.begin(), pathString.end(), '\\', '/');
				ImGui::SetDragDropPayload("Asset Load", pathString.c_str(), pathString.size() + 1);
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::End();
	}
}
