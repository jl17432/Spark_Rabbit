#pragma once
#include"AssetSerializer.h"
#include <map>
#include <unordered_map>

namespace SparkRabbit {
	class AssetTypes
	{
	public:
		static void Init();
		static AssetType GetAssetTypeFromExtension(const std::string& extension);

	private:
		static std::map<std::string, AssetType> s_Types;
	};

	class AssetManager
	{
	public:
		using AssetsChangeEventFn = std::function<void()>;
	public:
		static void Init();
		static void SetAssetChangeCallback(const AssetsChangeEventFn& callback);
		static void Shutdown();

		static bool IsAssetHandleValid(AssetHandle assetHandle);

		static AssetHandle GetAssetHandleFromFilePath(const std::string& filepath);


		template<typename T, typename... Args>
		static std::shared_ptr<T> CreateAsset(const std::string& filename, AssetType type, AssetHandle directoryHandle, Args&&... args)
		{
			static_assert(std::is_base_of<Asset, T>::value, "CreateAsset only works for types derived from Asset");

			auto& directory = GetAsset<Directory>(directoryHandle);

			std::filesystem::path p(filename);
			p.replace_extension("");
			std::filesystem::path q(filename);

			std::shared_ptr<T> asset = std::make_shared<T>(std::forward<Args>(args)...);
			asset->Type = type;
			asset->FilePath = directory->FilePath + "/" + filename;
			asset->FileName = p.string();
			asset->Extension = q.extension().string();
			asset->ParentDirectory = directoryHandle;
			asset->Handle = std::hash<std::string>()(asset->FilePath);
			asset->IsDataLoaded = true;
			s_LoadedAssets[asset->Handle] = asset;

			AssetSerializer::SerializeAsset(asset);

			return asset;
		}

		template<typename T>
		static std::shared_ptr<T> GetAsset(AssetHandle assetHandle, bool loadData = true)
		{
			SPARK_CORE_ASSERT(false,s_LoadedAssets.find(assetHandle) != s_LoadedAssets.end());
			std::shared_ptr<Asset> asset = s_LoadedAssets[assetHandle];

			if (!asset->IsDataLoaded && loadData)
				asset = AssetSerializer::LoadAssetData(asset);

			return std::make_shared<T>();
		}

		template<typename T>
		static std::shared_ptr<T> GetAsset(const std::string& filepath, bool loadData = true)
		{
			return GetAsset<T>(GetAssetHandleFromFilePath(filepath), loadData);
		}

		static bool IsAssetType(AssetHandle assetHandle, AssetType type)
		{
			return s_LoadedAssets.find(assetHandle) != s_LoadedAssets.end() && s_LoadedAssets[assetHandle]->Type == type;
		}
	private:
		static void ImportAsset(const std::string& filepath, AssetHandle parentHandle);
		//static void ReloadAssets();

	private:
		static std::unordered_map<AssetHandle, std::shared_ptr<Asset>> s_LoadedAssets;
		static AssetsChangeEventFn s_AssetsChangeCallback;
	};
}