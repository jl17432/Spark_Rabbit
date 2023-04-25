#include"PrecompileH.h"
#include"AssetManager.h"

#include"SparkRabbit/Asset/Asset.h"

namespace SparkRabbit {
	void AssetTypes::Init()
	{
		s_Types["hsc"] = AssetType::Scene;
		s_Types["fbx"] = AssetType::Mesh;
		s_Types["obj"] = AssetType::Mesh;
		s_Types["blend"] = AssetType::Mesh;
		s_Types["png"] = AssetType::Texture;
		s_Types["hdr"] = AssetType::EnvMap;
		s_Types["hpm"] = AssetType::PhysicsMat;
		s_Types["wav"] = AssetType::Audio;
		s_Types["ogg"] = AssetType::Audio;
		s_Types["cs"] = AssetType::Script;
	}

	AssetType AssetTypes::GetAssetTypeFromExtension(const std::string& extension)
	{
		return s_Types.find(extension) != s_Types.end() ? s_Types[extension] : AssetType::Other;
	}

	std::map<std::string, AssetType> AssetTypes::s_Types;

	void AssetManager::Init()
	{
		ReloadAssets();
	}

	void AssetManager::Shutdown()
	{
		s_LoadedAssets.clear();
	}

	bool AssetManager::IsAssetHandleValid(AssetHandle assetHandle)
	{
		return assetHandle != 0 && s_LoadedAssets.find(assetHandle) != s_LoadedAssets.end();
	}

	AssetHandle AssetManager::GetAssetHandleFromFilePath(const std::string& filepath)
	{
		for (auto& [id, asset] : s_LoadedAssets)
		{
			if (asset->FilePath == filepath)
				return id;
		}

		return 0;
	}

	void AssetManager::ImportAsset(const std::string& filepath, AssetHandle parentHandle)
	{
		std::filesystem::path p(filepath);
		std::string extension = p.extension().string().substr(1);
		if (extension == "meta")
			return;

		AssetType type = AssetTypes::GetAssetTypeFromExtension(extension);
		std::shared_ptr<Asset> asset = AssetSerializer::LoadAssetInfo(filepath, parentHandle, type);

		if (s_LoadedAssets.find(asset->Handle) != s_LoadedAssets.end())
		{
			if (s_LoadedAssets[asset->Handle]->IsDataLoaded)
			{
				asset = AssetSerializer::LoadAssetData(asset);
			}
		}

		s_LoadedAssets[asset->Handle] = asset;
	}

	AssetHandle AssetManager::ProcessDirectory(const std::string& directoryPath, AssetHandle parentHandle)
	{
		std::shared_ptr<Asset> temp = AssetSerializer::LoadAssetInfo(directoryPath, parentHandle, AssetType::Directory);
		std::shared_ptr<Directory> dirInfo = std::dynamic_pointer_cast<Directory>(temp);
		s_LoadedAssets[dirInfo->Handle] = dirInfo;

		if (IsAssetHandleValid(parentHandle))
			std::dynamic_pointer_cast<Directory>(s_LoadedAssets[parentHandle])->ChildDirectories.push_back(dirInfo->Handle);

		for (auto entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_directory())
				ProcessDirectory(entry.path().string(), dirInfo->Handle);
			else
				ImportAsset(entry.path().string(), dirInfo->Handle);
		}

		return dirInfo->Handle;
	}

	void AssetManager::ReloadAssets()
	{
		ProcessDirectory("assets", 0);
	}

	std::unordered_map<AssetHandle, std::shared_ptr<Asset>> AssetManager::s_LoadedAssets;
	AssetManager::AssetsChangeEventFn AssetManager::s_AssetsChangeCallback;


}