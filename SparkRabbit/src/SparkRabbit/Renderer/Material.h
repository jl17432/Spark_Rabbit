#pragma once
#include "SparkRabbit/Core.h"
#include "SparkRabbit/Renderer/Texture.h"
#include "SparkRabbit/Renderer/Shader.h"

#include <unordered_set>

#define BIT(x) (1 << x)
namespace SparkRabbit {

	//By using bitmasks, different flag bits can be combined to represent complex material properties. 
	//For example, combining DepthTest and Blend can represent enabling both depth testing and blending, 
	//with a value of binary number 00000110, which is decimal number 6.
	enum class MaterialFlag
	{
		None		= BIT(0),  // 0000 0000 0000 0000 0000 0000 0000 0001
		DepthTest	= BIT(1),  // 0000 0000 0000 0000 0000 0000 0000 0010
		Blend		= BIT(2),  // 0000 0000 0000 0000 0000 0000 0000 0100
	};

	class Material
	{
		friend class MaterialInstance;
	public:
		Material(const std::shared_ptr<Shader>& shader);
		virtual ~Material();

		void Bind() const;

		uint32_t GetFlags() const { return m_MaterialFlags; }
		void SetFlag(MaterialFlag flag) { m_MaterialFlags |= (uint32_t)flag; }

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = FindUniformDeclaration(name);
			SPARK_CORE_ASSERT(decl, "Could not find uniform with name '{0}'", name);
			auto& buffer = GetUniformBufferTarget(decl);
			buffer.Write((uint8_t*)&value, decl->GetSize(), decl->GetOffset()); 

			for (auto mi : m_MaterialInstances)
				mi->OnMaterialValueUpdated(decl);
		}

		void Set(const std::string& name, const std::shared_ptr<Texture>& texture)
		{
			auto decl = FindResourceDeclaration(name);
			uint32_t slot = decl->GetRegister();
			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		void Set(const std::string& name, const std::shared_ptr<Texture2D>& texture)
		{
			Set(name, (const std::shared_ptr<Texture>&)texture);
		}

		void Set(const std::string& name, const std::shared_ptr<TextureCube>& texture)
		{
			Set(name, (const std::shared_ptr<Texture>&)texture);
		}

		ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name);
	public:
		static std::shared_ptr<Material> Create(const std::shared_ptr<Shader>& shader);
	private:
		void AllocateStorage();
		void OnShaderReloaded();
		void BindTextures() const;

		ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name);
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
	private:
		std::shared_ptr<Shader> m_Shader;
		std::unordered_set<MaterialInstance*> m_MaterialInstances;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;
		std::vector<std::shared_ptr<Texture>> m_Textures;

		uint32_t m_MaterialFlags;
	};

	class MaterialInstance
	{
		friend class Material;
	public:
		MaterialInstance(const std::shared_ptr<Material>& material);
		virtual ~MaterialInstance();

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = m_Material->FindUniformDeclaration(name);
			if (!decl)
				return;
			SPARK_CORE_ASSERT(decl, "Could not find uniform with name '{0}'", name);
			//SPARK_CORE_ASSERT(decl, "Could not find uniform with name 'x'");
			auto& buffer = GetUniformBufferTarget(decl);
			buffer.Write((uint8_t*)&value, decl->GetSize(), decl->GetOffset()); 

			m_OverriddenValues.insert(name);
		}

		void Set(const std::string& name, const std::shared_ptr<Texture>& texture)
		{
			auto decl = m_Material->FindResourceDeclaration(name);
			if (!decl)
				SPARK_CORE_WARN("Cannot find material property: ", name);
			uint32_t slot = decl->GetRegister();
			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		void Set(const std::string& name, const std::shared_ptr<Texture2D>& texture)
		{
			Set(name, (const std::shared_ptr<Texture>&)texture);
		}

		void Set(const std::string& name, const std::shared_ptr<TextureCube>& texture)
		{
			Set(name, (const std::shared_ptr<Texture>&)texture);
		}

		void Bind() const;

		uint32_t GetFlags() const { return m_Material->m_MaterialFlags; }
		bool GetFlag(MaterialFlag flag) const { return (uint32_t)flag & m_Material->m_MaterialFlags; }
		void SetFlag(MaterialFlag flag, bool value = true);

		std::shared_ptr<Shader >GetShader() { return m_Material->m_Shader; }
	public:
		static std::shared_ptr<MaterialInstance> Create(const std::shared_ptr<Material>& material);
	private:
		void AllocateStorage();
		void OnShaderReloaded();
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
		void OnMaterialValueUpdated(ShaderUniformDeclaration* decl);
	private:
		std::shared_ptr<Material> m_Material;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;
		std::vector<std::shared_ptr<Texture>> m_Textures;

		// TODO: This is temporary; come up with a proper system to track overrides
		std::unordered_set<std::string> m_OverriddenValues;
	};

}

