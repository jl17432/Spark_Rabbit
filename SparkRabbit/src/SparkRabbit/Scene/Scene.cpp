#include "PrecompileH.h"
#include "Scene.h"
#include"Components.h"
#include"Entity.h"
#include"SparkRabbit/Math/Math.h"

#include "SparkRabbit/Renderer/SceneRenderer.h"
// box2d
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace SparkRabbit {

	static const std::string DefaultEntityName = "Entity";

	std::unordered_map<UUID, Scene*> s_ActiveScenes;

	struct SceneComponent
	{
		UUID SceneID;
	};

	// TODO: PHYSICS
	static b2BodyType RigidBody2DTypeToBox2DBody(RigidBody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case RigidBody2DComponent::BodyType::Static:		return b2_staticBody;
		case RigidBody2DComponent::BodyType::Dynamic:		return b2_dynamicBody;
		case RigidBody2DComponent::BodyType::Kinematic:	return b2_kinematicBody;
		}
		SPARK_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene(const std::string& debugName, bool isEditorScene)
		: m_DebugName(debugName)
	{
		m_SceneEntity = m_Registry.create();
		m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);

		s_ActiveScenes[m_SceneID] = this;

		Init();
	}

	Scene::~Scene()
	{
		m_Registry.clear();
		s_ActiveScenes.erase(m_SceneID);
	}

	void Scene::Init()
	{
		auto skyboxShader = Shader::Create("assets/shaders/Skybox.glsl");
		m_SkyboxMaterial = MaterialInstance::Create(Material::Create(skyboxShader));
		m_SkyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);
	}



	void Scene::OnUpdate(TickTime ts)
	{
		//TODO- physics here
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_2Dworld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			auto view = m_Registry.view<TransformComponent, RigidBody2DComponent>();

			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rigidBody2D = entity.GetComponent<RigidBody2DComponent>();

				b2Body* m_2Dbody = (b2Body*)rigidBody2D.RuntimeBody;

				const auto& position = m_2Dbody->GetPosition();
				std::cout << position.x << " " << position.y << std::endl;
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = m_2Dbody->GetAngle();
			}

		}
		{
			m_dynamicsWorld->stepSimulation(ts, 10);

			auto view = m_Registry.view<TransformComponent, Rigidbody3DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb3d = entity.GetComponent<Rigidbody3DComponent>();
				btRigidBody* body = (btRigidBody*)(rb3d).RuntimeBodyOJ;
				const auto& trans = body->getWorldTransform();
				(transform).Translation.x = trans.getOrigin().x();
				(transform).Translation.y = trans.getOrigin().y();
				(transform).Translation.z = trans.getOrigin().z();
				std::cout << trans.getOrigin().y() << std::endl;

				btScalar yawZ, pitchY, rollX;
				trans.getRotation().getEulerZYX(yawZ, pitchY, rollX);

				(transform).Rotation.x = rollX;
				(transform).Rotation.y = pitchY;
				(transform).Rotation.z = yawZ;
			}

			/*if (ModeManager::bShowPhysicsColliders)
			{
				Entity camera = mLevel->GetPrimaryCameraEntity();
				Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());

				m_dynamicsWorld->setDebugDrawer(&mDebugDrawer);
				m_dynamicsWorld->debugDrawWorld();

				Renderer2D::EndScene();
			}*/
		}
		//update all entities
		Camera* camera = nullptr;
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& comp = view.get<CameraComponent>(entity);
			camera = &comp.Camera;
			break;
		}
		SPARK_CORE_INFO("Scene::OnUpdate");


	}

	void Scene::OnRenderRuntime(TickTime ts)
	{
		Entity cameraEntity = GetMainCameraEntity();
		if (!cameraEntity)
			return;

		glm::mat4 cameraViewMatrix = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
		SPARK_CORE_ASSERT(cameraEntity, "Scene does not contain any cameras!");
		SceneCamera& camera = cameraEntity.GetComponent<CameraComponent>();
		camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);

		m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

		auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
		SceneRenderer::BeginScene(this, { camera, cameraViewMatrix });
		for (auto entity : group)
		{
			auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
			if (meshComponent.Mesh)
			{
				meshComponent.Mesh->OnUpdate(ts);

				// TODO: Should we render (logically)
				SceneRenderer::SubmitMesh(meshComponent, transformComponent, nullptr);
			}
		}
		SceneRenderer::EndScene();
		/////////////////////////////////////////////////////////////////////

#if 0
		// Render all sprites
		Renderer2D::BeginScene(*camera);
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderer>);
			for (auto entity : group)
			{
				auto [transformComponent, spriteRendererComponent] = group.get<TransformComponent, SpriteRenderer>(entity);
				if (spriteRendererComponent.Texture)
					Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Texture, spriteRendererComponent.TilingFactor);
				else
					Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Color);
			}
		}

		Renderer2D::EndScene();
#endif
	}
	//Editor mode
	void Scene::OnRenderEditor(TickTime ts, const ProjectiveCamera& projectiveCamera)
	{

		{
			m_LightEnvironment = LightEnvironment();
			auto lights = m_Registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
			uint32_t directionalLightIndex = 0;
			for (auto entity : lights)
			{
				auto [transformComponent, lightComponent] = lights.get<TransformComponent, DirectionalLightComponent>(entity);
				glm::vec3 direction = -glm::normalize(glm::mat3(transformComponent.GetTransform()) * glm::vec3(1.0f));
				m_LightEnvironment.DirectionalLights[directionalLightIndex++] =
				{
					direction,
					lightComponent.Radiance,
					lightComponent.Intensity,
					lightComponent.CastShadows
				};
			}
		}

		{
			m_Environment = std::make_shared<Environments>();
			auto lights = m_Registry.group<SkyLightComponent>(entt::get<TransformComponent>);
			for (auto entity : lights)
			{
				auto [transformComponent, skyLightComponent] = lights.get<TransformComponent, SkyLightComponent>(entity);
				m_Environment = skyLightComponent.SceneEnvironment;
				m_EnvironmentIntensity = skyLightComponent.Intensity;
				if (m_Environment)
					SetSkybox(m_Environment->RadianceMap);
			}
		}

		m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

		auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
		SceneRenderer::BeginScene(this, { projectiveCamera, projectiveCamera.GetView(), 0.1f, 1000.0f, 45.0f }); // TODO: real values
		for (auto entity : group)
		{
			auto& [meshComponent, transformComponent] = group.get<MeshComponent, TransformComponent>(entity);
			if (meshComponent.Mesh)
			{
				meshComponent.Mesh->OnUpdate(ts);

				glm::mat4 transform = GetTransformRelativeToParent(Entity{ entity, this });

				if (m_SelectedEntity == entity)
					SceneRenderer::SubmitSelectedMesh(meshComponent, transform);
				else
					SceneRenderer::SubmitMesh(meshComponent, transform);
			}
		}


		SceneRenderer::EndScene();
		/////////////////////////////////////////////////////////////////////

#if 0
		// Render all sprites
		Renderer2D::BeginScene(*camera);
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderer>);
			for (auto entity : group)
			{
				auto [transformComponent, spriteRendererComponent] = group.get<TransformComponent, SpriteRenderer>(entity);
				if (spriteRendererComponent.Texture)
					Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Texture, spriteRendererComponent.TilingFactor);
				else
					Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Color);
			}
		}

		Renderer2D::EndScene();
#endif
	}

	void Scene::OnEvent(Event& e)
	{
	}
	btVector3 glmToBtVec3(const glm::vec3& v)
	{
		return btVector3(v.x, v.y, v.z);
	}
	void Scene::OnRuntimeStart()
	{
		m_IsPlaying = true;
		// 2D physics
		m_2Dworld = new b2World({ 0.0f, -9.8f });
		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view) {
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidBody2D = entity.GetComponent<RigidBody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = RigidBody2DTypeToBox2DBody(rigidBody2D.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* m_2Dbody = m_2Dworld->CreateBody(&bodyDef);
			m_2Dbody->SetFixedRotation(rigidBody2D.FixedRotation);
			rigidBody2D.RuntimeBody = m_2Dbody;
			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& boxCollider2D = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(boxCollider2D.Size.x * transform.Scale.x, boxCollider2D.Size.y * transform.Scale.y);

				b2FixtureDef boxFixtureDef;
				boxFixtureDef.shape = &boxShape;
				boxFixtureDef.density = boxCollider2D.Density;
				boxFixtureDef.friction = boxCollider2D.Friction;
				boxFixtureDef.restitution = boxCollider2D.Restitution;
				boxFixtureDef.restitutionThreshold = boxCollider2D.RestitutionThreshold;
				m_2Dbody->CreateFixture(&boxFixtureDef);
			}
			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& circleCollider2D = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_radius = circleCollider2D.Radius * transform.Scale.x;
				circleShape.m_p = b2Vec2(circleCollider2D.Offset.x, circleCollider2D.Offset.y);

				b2FixtureDef circleFixtureDef;
				circleFixtureDef.shape = &circleShape;
				circleFixtureDef.density = circleCollider2D.Density;
				circleFixtureDef.friction = circleCollider2D.Friction;
				circleFixtureDef.restitution = circleCollider2D.Restitution;
				circleFixtureDef.restitutionThreshold = circleCollider2D.RestitutionThreshold;
				m_2Dbody->CreateFixture(&circleFixtureDef);
			}

		}

		// 3d physics
		m_broadphase = new btDbvtBroadphase();
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
		m_solver = new btSequentialImpulseConstraintSolver();
		m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
		m_dynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));

		auto view3 = m_Registry.view<Rigidbody3DComponent>();
		for (auto e : view3)
		{
			Entity entity = { e,this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb3d = entity.GetComponent<Rigidbody3DComponent>();


			btTransform bttransform;
			btCollisionShape* shape;
			btVector3 inertia{ 0.0f, 0.0f, 0.0f };

			//

			if (rb3d.Shape == CollisionShape::Box)
			{

				auto& transform = entity.GetComponent<TransformComponent>();
				auto& mesh = entity.GetComponent<MeshComponent>();

				std::vector<glm::vec3> vertices;
				/*for (const auto& subMesh : mesh.Mesh->GetSubmeshes())
				{
					if (subMesh.mStaticVertices.empty())
					{
						for (const auto& vertex : subMesh.mSkinnedVertices)
						{
							vertices.emplace_back(vertex.Pos);
						}
					}
					else
					{
						for (const auto& vertex : subMesh.mStaticVertices)
						{
							vertices.emplace_back(vertex.Pos);
						}
					}
				}*/
				for (const auto& vertex : mesh.Mesh->GetAnimatedVertices())
				{
					vertices.emplace_back(vertex.Position);
				}

				for (const auto& vertex : mesh.Mesh->GetStaticVertices())
				{
					vertices.emplace_back(vertex.Position);
				}

				glm::vec3 originPos(0.0f);
				glm::mat3 covMat = Math::CalculateCovMatrix(vertices, originPos);

				glm::vec3 eValues;
				glm::mat3 eVectors;
				Math::JacobiSolver(covMat, eValues, eVectors);

				for (int i = 0; i < 3; i++)
				{
					if (eValues[i] == 0 || i == 2)
					{
						Math::SchmidtOrthogonalization(eVectors[(i + 1) % 3], eVectors[(i + 2) % 3], eVectors[i]);
						break;
					}
				}

				constexpr float infinity = std::numeric_limits<float>::infinity();
				glm::vec3 minExtents(infinity, infinity, infinity);
				glm::vec3 maxExtents(-infinity, -infinity, -infinity);

				for (const glm::vec3& displacement : vertices)
				{
					minExtents[0] = std::min(minExtents[0], glm::dot(displacement, eVectors[0]));
					minExtents[1] = std::min(minExtents[1], glm::dot(displacement, eVectors[1]));
					minExtents[2] = std::min(minExtents[2], glm::dot(displacement, eVectors[2]));

					maxExtents[0] = std::max(maxExtents[0], glm::dot(displacement, eVectors[0]));
					maxExtents[1] = std::max(maxExtents[1], glm::dot(displacement, eVectors[1]));
					maxExtents[2] = std::max(maxExtents[2], glm::dot(displacement, eVectors[2]));
				}

				glm::vec3 halfExtent = (maxExtents - minExtents) / 2.0f;
				glm::vec3 offset = halfExtent + minExtents;
				originPos += offset.x * eVectors[0] + offset.y * eVectors[1] + offset.z * eVectors[2];
				glm::vec3 offsetScale = originPos * (transform.Scale - 1.0f);
				originPos += offsetScale;
				originPos = glm::mat3(transform.GetRotationMatrix()) * originPos;
				originPos += transform.Translation;

				shape = new btBoxShape(btVector3(halfExtent.x * transform.Scale.x, halfExtent.y * transform.Scale.y, halfExtent.z * transform.Scale.z));
				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);

				bttransform.setOrigin(glmToBtVec3(originPos));

				auto comQuat = glm::quat(transform.Rotation) * glm::quat(glm::mat4(eVectors));
				btQuaternion btQuat;
				btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
				bttransform.setRotation(btQuat);
			}
			else if (rb3d.Shape == CollisionShape::Sphere)
			{
				shape = new btSphereShape(transform.Scale.x);
				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);

				bttransform.setOrigin(btVector3(transform.Translation.x, transform.Translation.y, transform.Translation.z));

				auto comQuat = glm::quat(transform.Rotation);
				btQuaternion btQuat;
				btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
				bttransform.setRotation(btQuat);
			}
			else if (rb3d.Shape == CollisionShape::ConvexHull && entity.HasComponent<MeshComponent>())
			{
				auto& meshc = entity.GetComponent<MeshComponent>();

				shape = new btConvexHullShape();

				/*for (const auto& submesh : meshc.mMesh->mSubMeshes)
				{
					auto& staticVertices = submesh.mStaticVertices;
					auto& skinnedVertices = submesh.mSkinnedVertices;

					for (const auto& vertex : staticVertices)
					{
						static_cast<btConvexHullShape*>(shape)->addPoint(btVector3(vertex.Pos.x * transform.Scale.x, vertex.Pos.y * transform.Scale.y, vertex.Pos.z * transform.Scale.z));
					}
					for (const auto& vertex : skinnedVertices)
					{
						static_cast<btConvexHullShape*>(shape)->addPoint(btVector3(vertex.Pos.x * transform.Scale.x, vertex.Pos.y * transform.Scale.y, vertex.Pos.z * transform.Scale.z));
					}
				}*/

				auto& staticVertices = meshc.Mesh->GetStaticVertices();
				auto& skinnedVertices = meshc.Mesh->GetAnimatedVertices();

				for (const auto& vertex : staticVertices)
				{
					static_cast<btConvexHullShape*>(shape)->addPoint(btVector3(vertex.Position.x * transform.Scale.x, vertex.Position.y * transform.Scale.y, vertex.Position.z * transform.Scale.z));
				}
				for (const auto& vertex : skinnedVertices)
				{
					static_cast<btConvexHullShape*>(shape)->addPoint(btVector3(vertex.Position.x * transform.Scale.x, vertex.Position.y * transform.Scale.y, vertex.Position.z * transform.Scale.z));
				}


				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);

				bttransform.setOrigin(btVector3(transform.Translation.x, transform.Translation.y, transform.Translation.z));

				auto comQuat = glm::quat(transform.Rotation);
				btQuaternion btQuat;
				btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
				bttransform.setRotation(btQuat);
			}

			btDefaultMotionState* motion = new btDefaultMotionState(bttransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(rb3d.mass, motion, shape, inertia);
			rbInfo.m_linearDamping = rb3d.linearDamping;
			rbInfo.m_angularDamping = rb3d.angularDamping;
			rbInfo.m_restitution = rb3d.restitution;
			rbInfo.m_friction = rb3d.friction;

			btRigidBody* body = new btRigidBody(rbInfo);
			body->setSleepingThresholds(0.01f, glm::radians(0.1f));
			body->setActivationState(DISABLE_DEACTIVATION);

			if (rb3d.Type == Rigidbody3DComponent::BodyType::Static)
			{
				body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
			}
			else if (rb3d.Type == Rigidbody3DComponent::BodyType::Kinematic)
			{
				body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			}
			else if (rb3d.Type == Rigidbody3DComponent::BodyType::Dynamic)
			{
				body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
			}

			rb3d.RuntimeBodyOJ = body;
			m_dynamicsWorld->addRigidBody(body);
		}
	}

	void Scene::OnRuntimeStop()
	{
		m_IsPlaying = false;

		delete m_2Dworld;
		m_2Dworld = nullptr;

		delete m_dynamicsWorld;
		delete m_solver;
		delete m_dispatcher;
		delete m_collisionConfiguration;
		delete m_broadphase;
	}

	//TODO: RunTime start and stop

	void Scene::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	void Scene::SetSkybox(const std::shared_ptr <TextureCube>& skybox)
	{
		m_SkyboxTexture = skybox;
		m_SkyboxMaterial->Set("u_Texture", skybox);
	}

	Entity Scene::GetMainCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& comp = view.get<CameraComponent>(entity);
			if (comp.Primary)
				return { entity, this };
		}
		return {};
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = {};

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);

		entity.AddComponent<RelationshipComponent>();

		m_EntityIDMap[idComponent.ID] = entity;
		return entity;
	}

	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name, bool runtimeMap)
	{
		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = uuid;

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);

		entity.AddComponent<RelationshipComponent>();

		//SPARK_CORE_ASSERT(false,m_EntityIDMap.find(uuid) == m_EntityIDMap.end());
		m_EntityIDMap[uuid] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity.m_EntityHandle);
	}

	template<typename T>
	static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto components = srcRegistry.view<T>();
		for (auto srcEntity : components)
		{
			entt::entity destEntity = enttMap.at(srcRegistry.get<IDComponent>(srcEntity).ID);

			auto& srcComponent = srcRegistry.get<T>(srcEntity);
			auto& destComponent = dstRegistry.emplace_or_replace<T>(destEntity, srcComponent);
		}
	}

	template<typename T>
	static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry& registry)
	{
		if (registry.has<T>(src))
		{
			auto& srcComponent = registry.get<T>(src);
			registry.emplace_or_replace<T>(dst, srcComponent);
		}
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity;
		if (entity.HasComponent<TagComponent>())
			newEntity = CreateEntity(entity.GetComponent<TagComponent>().Tag);
		else
			newEntity = CreateEntity();

		CopyComponentIfExists<TransformComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<RelationshipComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<MeshComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<DirectionalLightComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<SkyLightComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<CameraComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<RigidBody2DComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
		CopyComponentIfExists<Rigidbody3DComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_Registry);
	}

	Entity Scene::FindEntityByTag(const std::string& tag)
	{
		// TODO: If this becomes used often, consider indexing by tag
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const auto& canditate = view.get<TagComponent>(entity).Tag;
			if (canditate == tag)
				return Entity(entity, this);
		}

		return Entity{};
	}

	Entity Scene::FindEntityByUUID(UUID id)
	{
		auto view = m_Registry.view<IDComponent>();
		for (auto entity : view)
		{
			auto& idComponent = m_Registry.get<IDComponent>(entity);
			if (idComponent.ID == id)
				return Entity(entity, this);
		}

		return Entity{};
	}

	glm::mat4 Scene::GetTransformRelativeToParent(Entity entity)
	{
		glm::mat4 transform(1.0F);

		Entity parent = FindEntityByUUID(entity.GetParentUUID());
		if (parent)
			transform = GetTransformRelativeToParent(parent);

		return transform * entity.Transform().GetTransform();
	}

	// Copy to runtime
	void Scene::CopyTo(std::shared_ptr<Scene>& target)
	{
		// Environment
		target->m_Light = m_Light;
		target->m_LightMultiplier = m_LightMultiplier;

		target->m_Environment = m_Environment;
		target->m_SkyboxTexture = m_SkyboxTexture;
		target->m_SkyboxMaterial = m_SkyboxMaterial;
		target->m_SkyboxLod = m_SkyboxLod;

		std::unordered_map<UUID, entt::entity> enttMap;
		auto idComponents = m_Registry.view<IDComponent>();
		for (auto entity : idComponents)
		{
			auto uuid = m_Registry.get<IDComponent>(entity).ID;
			Entity e = target->CreateEntityWithID(uuid, "", true);
			enttMap[uuid] = e.m_EntityHandle;
		}

		CopyComponent<TagComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<TransformComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<RelationshipComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<MeshComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<DirectionalLightComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<SkyLightComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<CameraComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<SpriteRendererComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<RigidBody2DComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<BoxCollider2DComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<CircleCollider2DComponent>(target->m_Registry, m_Registry, enttMap);
		CopyComponent<Rigidbody3DComponent>(target->m_Registry, m_Registry, enttMap);
	}
}
