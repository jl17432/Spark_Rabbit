#pragma once
//#include <glad/glad.h>


#include "SparkRabbit/Buffer.h"

#include "SparkRabbit/Renderer/RenderAPI.h"
#include "SparkRabbit/Renderer/ShaderUniform.h"

#include <glm/glm.hpp>

namespace SparkRabbit {

	struct ShaderUniform
	{

	};

	struct ShaderUniformCollection
	{

	};

	enum class UniformType
	{
		None = 0,
		Float, Vec2, Vec3, Vec4,
		Mat3, Mat4,
		Int32, Uint32
	};

	struct UniformDecl
	{
		UniformType Type;
		std::ptrdiff_t Offset;
		std::string Name;
	};

	struct UniformBuffer
	{
		//this is simply a CPU-side buffer abstraction.
		uint8_t* Buffer;
		std::vector<UniformDecl> Uniforms;
	};

	struct UniformBufferBase
	{
		virtual const uint8_t* GetBuffer() const = 0;
		virtual const UniformDecl* GetUniforms() const = 0;
		virtual unsigned int GetUniformCount() const = 0;
	};

	template<unsigned int N, unsigned int U>
	struct UniformBufferDeclaration : public UniformBufferBase
	{
		uint8_t Buffer[N]; //N is the size of the buffer
		UniformDecl Uniforms[U]; //U is the number of uniforms
		std::ptrdiff_t Cursor = 0; //the current position in the buffer 
		int Index = 0; //the current index in the Uniforms array

		virtual const uint8_t* GetBuffer() const override { return Buffer; }
		virtual const UniformDecl* GetUniforms() const override { return Uniforms; }
		virtual unsigned int GetUniformCount() const { return U; }

		template<typename T>
		void Push(const std::string& name, const T& data) {}

		template<>
		void Push(const std::string& name, const float& data)
		{
			Uniforms[Index++] = { UniformType::Float, Cursor, name };
			memcpy(Buffer + Cursor, &data, sizeof(float));
			Cursor += sizeof(float);
		}

		template<>
		void Push(const std::string& name, const glm::vec3& data)
		{
			Uniforms[Index++] = { UniformType::Vec3, Cursor, name };  
			memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec3));
			Cursor += sizeof(glm::vec3);
		}

		template<>
		void Push(const std::string& name, const glm::vec4& data)
		{
			Uniforms[Index++] = { UniformType::Vec4, Cursor, name }; 
			memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec4));
			Cursor += sizeof(glm::vec4);
		}

		template<>
		void Push(const std::string& name, const glm::mat4& data)
		{
			Uniforms[Index++] = { UniformType::Mat4, Cursor, name }; 
			memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::mat4));
			Cursor += sizeof(glm::mat4);
		}

	};

	class Shader
	{
	public:
		using ShaderReloadedCallback = std::function<void()>;

		virtual void Reload() = 0;

		virtual void Bind() = 0;
		virtual uint32_t GetRendererID() const = 0;
		virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) = 0;

		// Temporary while we don't have materials
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind = true) = 0;

		virtual void SetIntArray(const std::string& name, int* values, uint32_t size) = 0;

		virtual const std::string& GetName() const = 0;

		// Represents a complete shader program stored in a single file.
		// Note: currently for simplicity this is simply a string filepath, however
		//       in the future this will be an asset object + metadata
		static std::shared_ptr<Shader> Create(const std::string& filepath);
		static std::shared_ptr<Shader> CreateFromString(const std::string& source);

		virtual void SetVSMaterialUniformBuffer(Buffer buffer) = 0;
		virtual void SetPSMaterialUniformBuffer(Buffer buffer) = 0;

		virtual const ShaderUniformBufferList& GetVSRendererUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetPSRendererUniforms() const = 0;
		virtual bool HasVSMaterialUniformBuffer() const = 0;
		virtual bool HasPSMaterialUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const = 0;

		virtual const ShaderResourceList& GetResources() const = 0;

		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) = 0;

		// Temporary, before we have an asset manager
		static std::vector<std::shared_ptr<Shader>> s_ShaderList;
	};

	// This should be eventually handled by the Asset Manager
	class ShaderLibrary
	{
	public:
		ShaderLibrary();
		~ShaderLibrary();

		void Add(const std::shared_ptr<Shader>& shader);
		void Load(const std::string& path);
		void Load(const std::string& name, const std::string& path);

		std::shared_ptr<Shader>& Get(const std::string& name);
	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}