#pragma once
#include <string>
#include <filesystem>

#include"SparkRabbit/Log.h"

namespace SparkRabbit {
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path ScriptModulePath;
	};

	class AssetPath
	{
	public:
		static const std::filesystem::path& GetProjectDirectory()
		{
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			return GetAssetDirectory() / path;
		}

		ProjectConfig& GetConfig() { return m_Config; }

		static std::shared_ptr<AssetPath> GetActive() { return s_ActiveProject; }

		static std::shared_ptr<AssetPath> New();
		static std::shared_ptr<AssetPath> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		inline static std::shared_ptr<AssetPath> s_ActiveProject;
	};
}

