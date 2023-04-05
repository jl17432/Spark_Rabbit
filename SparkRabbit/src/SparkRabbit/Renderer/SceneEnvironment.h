#pragma once

#include "Texture.h"

namespace SparkRabbit {
	class Environments : public Asset
	{
	public:
		std::shared_ptr<TextureCube> RadianceMap;
		std::shared_ptr<TextureCube> IrradianceMap;

		static Environments Load(const std::string& path);

		Environments() = default;
		Environments(const std::shared_ptr<TextureCube>& radianceMap, const std::shared_ptr<TextureCube>& irradianceMap)
			: RadianceMap(radianceMap), IrradianceMap(irradianceMap) {}
	};
}

