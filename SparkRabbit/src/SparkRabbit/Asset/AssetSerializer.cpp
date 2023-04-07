#include "PrecompileH.h"
#include "AssetSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include"SparkRabbit/Renderer/Mesh.h"
#include"SparkRabbit/Renderer/SceneRenderer.h"
#include"SparkRabbit/Renderer/SceneEnvironment.h"

namespace SparkRabbit {

	std::shared_ptr<Asset> AssetSerializer::LoadAssetInfo(const std::string& filepath, AssetHandle parentHandle, AssetType type)
	{
		std::shared_ptr<Asset> asset = std::make_shared<Asset>();

		if (type == AssetType::Directory)
			asset = std::make_shared<Directory>();

		std::filesystem::path p(filepath);
		std::string extension = p.extension().string();
		asset->FilePath = filepath;
		std::replace(asset->FilePath.begin(), asset->FilePath.end(), '\\', '/');

		bool hasMeta = std::filesystem::exists(asset->FilePath + ".meta");
		if (hasMeta)
		{
			LoadMetaData(asset);
		}
		else
		{
			asset->Handle = AssetHandle();
			asset->Type = type;
		}
		p.replace_extension("");
		if (extension == "")
		{
			asset->Extension = extension;
		}
		else {
			asset->Extension = extension.substr(1);
		}
		std::filesystem::path temp(asset->FilePath);
		asset->FileName = temp.stem().string();
		asset->ParentDirectory = parentHandle;
		asset->IsDataLoaded = false;

		if (!hasMeta)
			CreateMetaFile(asset);

		return asset;
	}

	std::shared_ptr<Asset> AssetSerializer::LoadAssetData(std::shared_ptr<Asset>& asset)
	{
		if (asset->Type == AssetType::Directory)
			return asset;

		std::shared_ptr<Asset> temp = asset;
		bool loadYAMLData = true;

		switch (asset->Type)
		{
		case AssetType::Mesh:
		{
			if (asset->Extension != "blend")
				asset = std::make_shared<Mesh>(asset->FilePath);
			loadYAMLData = false;
			break;
		}
		case AssetType::Texture:
		{
			asset = Texture2D::Create(asset->FilePath);
			loadYAMLData = false;
			break;
		}
		case AssetType::EnvMap:
		{
			auto [radiance, irradiance] = SceneRenderer::CreateEnvironmentMap(asset->FilePath);
			asset = std::make_shared<Environments>(radiance, irradiance);
			loadYAMLData = false;
			break;
		}
		case AssetType::Scene:
		case AssetType::Audio:
		case AssetType::Script:
		case AssetType::Other:
		{
			loadYAMLData = false;
			break;
		}
		}

		asset->Handle = temp->Handle;
		asset->FilePath = temp->FilePath;
		asset->FileName = temp->FileName;
		asset->Extension = temp->Extension;
		asset->ParentDirectory = temp->ParentDirectory;
		asset->Type = temp->Type;
		asset->IsDataLoaded = true;

		return asset;
	}

	void AssetSerializer::LoadMetaData(std::shared_ptr<Asset>& asset)
	{
		std::ifstream stream(asset->FilePath + ".meta");
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Asset"])
			SPARK_CORE_ASSERT(FALSE,"Invalid File Format");

		asset->Handle = data["Asset"].as<uint64_t>();
		asset->FilePath = data["FilePath"].as<std::string>();
		asset->Type = (AssetType)data["Type"].as<int>();

		if (asset->FileName == "assets" && asset->Handle == 0)
		{
			asset->Handle = AssetHandle();
			CreateMetaFile(asset);
		}
	}
	void AssetSerializer::CreateMetaFile(const std::shared_ptr<Asset>& asset)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Asset" << YAML::Value << asset->Handle;
		out << YAML::Key << "FilePath" << YAML::Value << asset->FilePath;
		out << YAML::Key << "Type" << YAML::Value << (int)asset->Type;
		out << YAML::EndMap;

		std::ofstream fout(asset->FilePath + ".meta");
		fout << out.c_str();
	}
	void AssetSerializer::SerializeAsset(const std::shared_ptr<Asset>& asset, AssetType type)
	{
		YAML::Emitter out;

		std::ofstream fout(asset->FilePath);
		fout << out.c_str();
	}
	std::shared_ptr<Asset> AssetSerializer::DeserializeYAML(const std::shared_ptr<Asset>& asset)
	{
		std::ifstream stream(asset->FilePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		return nullptr;
	}
}
