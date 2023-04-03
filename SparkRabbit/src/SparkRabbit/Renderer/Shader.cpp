#include "PrecompileH.h"
#include "Shader.h"

#include "SparkRabbit/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace SparkRabbit {
	
	std::vector<std::shared_ptr<Shader>> Shader::s_ShaderList;

	std::shared_ptr<Shader> Shader::Create(const std::string& filepath)
	{
		std::shared_ptr<Shader> result = nullptr;
		switch (RenderAPI::GetAPI())
		{
			case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RenderAPI::API::OpenGL:  result = std::make_shared<OpenGLShader>(filepath);
		}
		s_ShaderList.push_back(result);

		return result;
	}

	std::shared_ptr<Shader> Shader::CreateFromString(const std::string& source)
	{
		std::shared_ptr<Shader> result = nullptr; 
		switch (RenderAPI::GetAPI()) 
		{
		case RenderAPI::API::None:    SPARK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  result = std::make_shared<OpenGLShader>(source);
		}
		s_ShaderList.push_back(result);

		return result;
	}
	
	
	ShaderLibrary::ShaderLibrary()
	{
	}
	ShaderLibrary::~ShaderLibrary()
	{
	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
	{
		auto& name = shader->GetName();
		SPARK_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Load(const std::string& path)
	{
		auto& shader = std::shared_ptr<Shader>(Shader::Create(path));
		auto& name = shader->GetName();
		SPARK_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Load(const std::string& name, const std::string& path)
	{
		auto& shader = std::shared_ptr<Shader>(Shader::Create(path)); 
		SPARK_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader already exists!"); 
		m_Shaders[name] = shader; 
	}

	std::shared_ptr<Shader>& ShaderLibrary::Get(const std::string& name)
	{
		SPARK_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "Couldn't find the shader, shader name:{0}", name);
		return m_Shaders[name];
	}
}