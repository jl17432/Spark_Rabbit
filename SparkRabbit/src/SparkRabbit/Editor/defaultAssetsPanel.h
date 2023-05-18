#pragma once
#include "SparkRabbit/Renderer/Texture.h"
#include "SparkRabbit/Asset/AssetManager.h"

namespace SparkRabbit {
	class defaultAssetsPanel
	{
	public:
		defaultAssetsPanel();

		void OnImGuiRender();

	private:
		void DrawObject(const char* label, std::string path);

	private:
		std::shared_ptr<Texture2D> m_CubeImage;
	};
}