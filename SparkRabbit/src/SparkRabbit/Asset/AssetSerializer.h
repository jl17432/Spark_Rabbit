#pragma once
#include"Asset.h"
namespace SparkRabbit {
	class AssetSerializer
	{
	public:
		template<typename T>
		static void SerializeAsset(const std::shared_ptr<T>& asset)
		{
			static_assert(std::is_base_of<Asset, T>::value, "SerializeAsset only accepts types that inherit from Asset");
			SerializeAsset(asset, asset->Type);
		}

		static std::shared_ptr<Asset> LoadAssetInfo(const std::string& filepath, AssetHandle parentHandle, AssetType type);
		static std::shared_ptr<Asset> LoadAssetData(std::shared_ptr<Asset>& asset);

		static void LoadMetaData(std::shared_ptr<Asset>& asset);
		static void CreateMetaFile(const std::shared_ptr<Asset>& asset);
	private:
		static void SerializeAsset(const std::shared_ptr<Asset>& asset, AssetType type);
		static std::shared_ptr<Asset> DeserializeYAML(const std::shared_ptr<Asset>& asset);
	};
}

