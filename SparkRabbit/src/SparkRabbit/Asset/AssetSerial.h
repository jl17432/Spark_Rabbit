#pragma once
#include"AssetPath.h"
namespace SparkRabbit {
	class AssetSerial
	{
	public:
		AssetSerial(std::shared_ptr<AssetPath> project);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		std::shared_ptr<AssetPath> m_Project;
	};
}

