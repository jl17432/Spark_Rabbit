#pragma once
#include"Platform/OpenGL/OpenGLShader.h"

namespace SparkRabbit {
	struct PipelineSpecification
	{
		std::shared_ptr<SparkRabbit::Shader> Shader;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual const PipelineSpecification& GetSpecification() const = 0;

		virtual void Invalidate() = 0;

		// TEMP: remove this when render command buffers are a thing
		virtual void Bind() = 0;

		static std::shared_ptr<Pipeline> Create(const PipelineSpecification& spec);
	};
}