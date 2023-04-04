#pragma once
#include <glm/glm.hpp>
#include "SparkRabbit/Renderer/Mesh.h"

namespace SparkRabbit {
	class Entity
	{
	public:
		~Entity();

		// TODO: Move to Component -- future ECS system
		void SetMesh(const std::shared_ptr<Mesh>& mesh) { m_Mesh = mesh; }
		std::shared_ptr<Mesh> GetMesh() { return m_Mesh; }

		void SetMaterial(const std::shared_ptr<MaterialInstance>& material) { m_Material = material; }
		std::shared_ptr<MaterialInstance> GetMaterial() { return m_Material; }

		const glm::mat4& GetTransform() const { return m_Transform; }
		glm::mat4& Transform() { return m_Transform; }

		const std::string& GetName() const { return m_Name; }
	private:
		Entity(const std::string& name);
	private:
		std::string m_Name;
		glm::mat4 m_Transform;

		std::shared_ptr<Mesh> m_Mesh;
		std::shared_ptr<MaterialInstance> m_Material;

		friend class Scene;
	};

}

