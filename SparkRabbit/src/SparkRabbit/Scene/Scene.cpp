#include "PrecompileH.h"
#include "Scene.h"
#include"Components.h"
#include"Entity.h"
#include"SparkRabbit/Math/Math.h"
#include"SparkRabbit/Input.h"

#include "SparkRabbit/Renderer/SceneRenderer.h"
#include"imgui.h"
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

	glm::vec2 GetMouseDelta()
	{
		static ImVec2 lastPos = ImGui::GetMousePos();

		ImVec2 currentPos = ImGui::GetMousePos();
		glm::vec2 delta(0.0f, 0.0f);
		if (ImGui::IsMouseDown(0))
		{
			delta.x = currentPos.x - lastPos.x;
			delta.y = currentPos.y - lastPos.y;
		}


		lastPos = currentPos;

		return delta;
	}

	void Scene::Update3DBodies(TickTime ts)
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
			transform.Translation.x = trans.getOrigin().x();
			transform.Translation.y = trans.getOrigin().y();
			transform.Translation.z = trans.getOrigin().z();

			btScalar yawZ, pitchY, rollX;
			trans.getRotation().getEulerZYX(yawZ, pitchY, rollX);
			transform.Rotation.x = rollX;
			transform.Rotation.y = pitchY;
			transform.Rotation.z = yawZ;
		}
	}

	Camera* Scene::FindFirstCameraInScene()
	{
		Camera* camera = nullptr;
		auto view = m_Registry.view<CameraComponent>();
		if (!view.empty())
		{
			auto entity = *view.begin();
			auto& comp = view.get<CameraComponent>(entity);
			camera = &comp.Camera;
		}
		return camera;
	}

	void Scene::OnUpdate(TickTime ts)
	{
		Update3DBodies(ts);

		//update all entities
		Camera* camera = FindFirstCameraInScene();

	}

	void Scene::OnEvent(Event& e)
	{
	}

	// updates the camera's orientation and position based on user input.
	void Scene::UpdateCameraMovement(TickTime ts, Entity& cameraEntity)
	{
		auto& transform = cameraEntity.GetComponent<TransformComponent>();

		float sensitivity = 0.1f;
		glm::vec2 mouseDelta = GetMouseDelta();
		transform.Rotation.y += mouseDelta.x * sensitivity * ts;
		transform.Rotation.x += mouseDelta.y * sensitivity * ts;

		glm::mat4 rotation = transform.GetRotationMatrix();

		transform.Forward = glm::normalize(glm::mat3(rotation) * glm::vec3(0.0f, 0.0f, -1.0f));
		transform.Right = glm::normalize(glm::mat3(rotation) * glm::vec3(1.0f, 0.0f, 0.0f));
		transform.Up = glm::normalize(glm::mat3(rotation) * glm::vec3(0.0f, 1.0f, 0.0f));

		float speed = 5.0f;
		if (!Input::IsKeyPressed(SR_KEY_W))    transform.Translation -= speed * ts * transform.Forward;
		if (!Input::IsKeyPressed(SR_KEY_S))  transform.Translation += speed * ts * transform.Forward;
		if (!Input::IsKeyPressed(SR_KEY_LEFT_SHIFT))    transform.Translation -= speed * ts * transform.Up;
		if (!Input::IsKeyPressed(SR_KEY_LEFT_CONTROL))  transform.Translation += speed * ts * transform.Up;
		if (!Input::IsKeyPressed(SR_KEY_A))  transform.Translation += speed * ts * transform.Right;
		if (!Input::IsKeyPressed(SR_KEY_D)) transform.Translation -= speed * ts * transform.Right;
	}

	// This function sets up directional lights in the scene. It iterates over all entities with DirectionalLightComponent
    // and stores their properties into the LightEnvironment struct for use in the rendering process.
	void Scene::SetupDirectionalLights()
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

	//iterates over all entities with a MeshComponent and TransformComponent, updates the mesh and submits it for rendering.
	void Scene::RenderMeshesInScene(TickTime ts)
	{
		auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
			if (meshComponent.Mesh)
			{
				meshComponent.Mesh->OnUpdate(ts);

				glm::mat4 transform = GetTransformRelativeToParent(Entity(entity, this));

				SceneRenderer::SubmitMesh(meshComponent, transform, nullptr);
			}
		}
	}

	//responsible for rendering the scene in runtime mode. It first retrieves the main camera entity and updates its movement.
	void Scene::OnRenderRuntime(TickTime ts)
	{
		Entity cameraEntity = GetMainCameraEntity();
		SPARK_CORE_ASSERT(cameraEntity, "Scene does not contain any cameras!");

		UpdateCameraMovement(ts, cameraEntity);
		glm::mat4 cameraViewMatrix = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

		SceneCamera& camera = cameraEntity.GetComponent<CameraComponent>();
		camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);

		SetupDirectionalLights();

		m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

		SceneRenderer::BeginScene(this, { camera, cameraViewMatrix });
		RenderMeshesInScene(ts);
		SceneRenderer::EndScene();
	}

	void Scene::SetupSkyLight()
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

	//Editor mode
	void Scene::OnRenderEditor(TickTime ts, const ProjectiveCamera& projectiveCamera)
	{
		SetupDirectionalLights();
		SetupSkyLight();

		m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLod);

		SceneRenderer::BeginScene(this, { projectiveCamera, projectiveCamera.GetView(), 0.1f, 1000.0f, 45.0f });
		RenderMeshesInScene(ts);
		SceneRenderer::EndScene();
	}


	btVector3 glmToBtVec3(const glm::vec3& v)
	{
		return btVector3(v.x, v.y, v.z);
	}

	void Scene::CreateCircleFixture(Entity& entity) 
	{
		// Get necessary components
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rigidBody2D = entity.GetComponent<RigidBody2DComponent>();
		auto& circleCollider2D = entity.GetComponent<CircleCollider2DComponent>();

		// Define the circle shape
		b2CircleShape circleShape;
		circleShape.m_radius = circleCollider2D.Radius * transform.Scale.x;
		circleShape.m_p = b2Vec2(circleCollider2D.Offset.x, circleCollider2D.Offset.y);

		// Define the circle fixture
		b2FixtureDef circleFixtureDef;
		circleFixtureDef.shape = &circleShape;
		circleFixtureDef.density = circleCollider2D.Density;
		circleFixtureDef.friction = circleCollider2D.Friction;
		circleFixtureDef.restitution = circleCollider2D.Restitution;
		circleFixtureDef.restitutionThreshold = circleCollider2D.RestitutionThreshold;

		// Get the rigid body from the runtime and create the fixture
		b2Body* m_2Dbody = (b2Body*)rigidBody2D.RuntimeBody;
		m_2Dbody->CreateFixture(&circleFixtureDef);
	}

	void Scene::Init3DPhysics() {
		// Initialize Bullet physics
		m_broadphase = new btDbvtBroadphase();
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
		m_solver = new btSequentialImpulseConstraintSolver();
		m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
		m_dynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));

		// Get all entities with a Rigidbody3DComponent
		auto view3 = m_Registry.view<Rigidbody3DComponent>();

		// Iterate over all entities and create a 3D body for each
		for (auto e : view3) {
			Entity entity = { e,this };
			Create3DBody(entity);
		}
	}

	void Scene::Create3DBody(Entity& entity) {
		// Get necessary components
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb3d = entity.GetComponent<Rigidbody3DComponent>();

		// Convert glm::quat to btQuaternion
		auto to_btQuaternion = [](const glm::quat& q) {
			return btQuaternion(q.x, q.y, q.z, q.w);
		};

		// Convert glm::vec3 to btVector3
		auto to_btVector3 = [](const glm::vec3& v) {
			return btVector3(v.x, v.y, v.z);
		};

		// Configure the body
		btTransform bttransform;
		//bttransform.setOrigin(to_btVector3(transform.Translation));
		//bttransform.setRotation(to_btQuaternion(transform.Rotation));

		btCollisionShape* shape;
		btVector3 inertia{ 0.0f, 0.0f, 0.0f };

		//

		if (rb3d.Shape == CollisionShape::Box)
		{

			auto& transform = entity.GetComponent<TransformComponent>();
			auto& mesh = entity.GetComponent<MeshComponent>();

			std::vector<glm::vec3> vertices;
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
		}

		// Calculate inertia if mass is greater than zero
		if (rb3d.mass > 0.0f) {
			shape->calculateLocalInertia(rb3d.mass, inertia);
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

		// Set the collision flags based on the body type
		if (rb3d.Type == Rigidbody3DComponent::BodyType::Static) {
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
		else if (rb3d.Type == Rigidbody3DComponent::BodyType::Kinematic) {
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		}
		else if (rb3d.Type == Rigidbody3DComponent::BodyType::Dynamic) {
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
		}

		// Add the body to the dynamics world
		m_dynamicsWorld->addRigidBody(body);

		// Store the body in the runtime object
		rb3d.RuntimeBodyOJ = body;
	}


	void Scene::OnRuntimeStart()
	{
		// Initialize 2D physics
		//Init2DPhysics();

		// Initialize 3D physics
		Init3DPhysics();

		m_IsPlaying = true;
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

	// Set the viewport size
	void Scene::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	// Set the skybox texture
	void Scene::SetSkybox(const std::shared_ptr <TextureCube>& skybox)
	{
		m_SkyboxTexture = skybox;
		m_SkyboxMaterial->Set("u_Texture", skybox);
	}

	// Get the main camera entity
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

	// Create an entity
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

	// Create an entity with a given UUID
	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name, bool runtimeMap)
	{
		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = uuid;

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);

		entity.AddComponent<RelationshipComponent>();
		m_EntityIDMap[uuid] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity.m_EntityHandle);
	}

	// Copy a component from one entity to another
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

	// Copy a component from one entity to another if the component exists
	template<typename T>
	static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry& registry)
	{
		if (registry.has<T>(src))
		{
			auto& srcComponent = registry.get<T>(src);
			registry.emplace_or_replace<T>(dst, srcComponent);
		}
	}

	// Duplicate an entity
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
