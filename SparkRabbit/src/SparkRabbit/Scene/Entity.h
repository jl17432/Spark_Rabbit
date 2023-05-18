#pragma once
#include <glm/glm.hpp>
#include "SparkRabbit/Renderer/Mesh.h"
#include"Components.h"
#include"Scene.h"

namespace SparkRabbit {
    class Entity {
    public:
        // constructors
        Entity() = default;
        Entity(entt::entity handle, Scene* scene)
            : m_EntityHandle(handle), m_Scene(scene) {}

        ~Entity() {}

        // Add a component of type T to the entity
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            SPARK_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        // Get a component of type T from the entity
        template<typename T>
        T& GetComponent() {
            SPARK_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        // Check if the entity has a component of type T
        template<typename T>
        bool HasComponent() {
            return m_Scene->m_Registry.has<T>(m_EntityHandle);
        }

        // Remove a component of type T from the entity
        template<typename T>
        void RemoveComponent() {
            SPARK_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        // Get the TransformComponent of the entity
        TransformComponent& Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }
        const glm::mat4& Transform() const { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle).GetTransform(); }

        // Type conversion operators
        operator uint32_t () const { return (uint32_t)m_EntityHandle; }
        operator entt::entity() const { return m_EntityHandle; }
        operator bool() const { return (uint32_t)m_EntityHandle && m_Scene; }

        // Equality operators
        bool operator==(const Entity& other) const {
            return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
        }
        bool operator!=(const Entity& other) const {
            return !(*this == other);
        }

        // Set and get parent UUID
        void SetParentUUID(UUID parent) { GetComponent<RelationshipComponent>().ParentHandle = parent; }
        UUID GetParentUUID() { return GetComponent<RelationshipComponent>().ParentHandle; }

        // Get children UUIDs
        std::vector<UUID>& Children() { return GetComponent<RelationshipComponent>().Children; }

        // Check if the entity has a parent
        bool HasParent() { return m_Scene->FindEntityByUUID(GetParentUUID()); }

        // Check if the entity is an ancestor of the given entity
        bool IsAncestorOf(Entity entity) {
            const auto& children = Children();
            if (children.empty()) {
                return false;
            }

            if (std::find(children.begin(), children.end(), entity.GetUUID()) != children.end()) {
                return true;
            }

            return std::any_of(children.begin(), children.end(), [&](const UUID& child) {
                return m_Scene->FindEntityByUUID(child).IsAncestorOf(entity);
                });
        }

        // Check if the entity is a descendant of the given entity
        bool IsDescendantOf(Entity entity) {
            return entity.IsAncestorOf(*this);
        }

        // Get UUID of the entity and its scene
        UUID GetUUID() { return GetComponent<IDComponent>().ID; }
        UUID GetSceneUUID() { return m_Scene->GetUUID(); }

    private:
        Entity(const std::string& name);

    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene* m_Scene = nullptr;

        // Friends have access to private members
        friend class Scene;
        friend class SceneSerializer;
        friend class ScriptEngine;
    };



}

