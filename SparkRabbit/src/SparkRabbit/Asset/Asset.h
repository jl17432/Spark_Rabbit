#pragma once
#include"SparkRabbit/UUID.h"
#include<entt/include/entt.hpp>

namespace SparkRabbit {
	enum class AssetType
	{
		Scene,
		Mesh,
		Texture,
		EnvMap,
		Audio,
		Script,
		PhysicsMat,
		Directory,
		Other,
		None
	};

	using AssetHandle = UUID;

	class Asset
	{
	public:
		AssetHandle Handle;
		AssetType Type = AssetType::None;

		std::string FilePath;
		std::string FileName;
		std::string Extension;
		AssetHandle ParentDirectory;
		bool IsDataLoaded = false;

		virtual ~Asset() {}
	};

	class PhysicsMaterial : public Asset
	{
	public:
		float StaticFriction;
		float DynamicFriction;
		float Bounciness;

		PhysicsMaterial() = default;
		PhysicsMaterial(float staticFriction, float dynamicFriction, float bounciness)
			: StaticFriction(staticFriction), DynamicFriction(dynamicFriction), Bounciness(bounciness)
		{
		}
	};

	// Treating directories as assets simplifies the asset manager window rendering by a lot
	class Directory : public Asset
	{
	public:
		std::vector<AssetHandle> ChildDirectories;

		Directory() = default;
	};
}