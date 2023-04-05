#include "PrecompileH.h"
#include "SceneEnvironment.h"
#include "SceneRenderer.h"

namespace SparkRabbit {

	Environments Environments::Load(const std::string& filepath)
	{
		auto [radiance, irradiance] = SceneRenderer::CreateEnvironmentMap(filepath);
		return { radiance, irradiance };
	}
}