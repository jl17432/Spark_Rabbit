#pragma once
#include "SparkRabbit/Renderer/Texture.h"
#include <filesystem>

namespace SparkRabbit {
	class FileSystemPanel
	{
	public:
		FileSystemPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;

		std::shared_ptr<Texture2D> m_DirectoryIcon;
		std::shared_ptr<Texture2D> m_FileIcon;
	};
}


