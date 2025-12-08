#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "EditorEvents.h"
#include "EditorImGui.h"
#include "UndoRedoAction.h"

#include <Enigma/Core/Process/Application.h>
#include <Enigma/Engine/ECS/Components.h>

#include <imgui.h>
#include <imgui_internal.h>

using namespace Enigma::Engine::ECS;

namespace Enigma::Editor {

	ref<Scene> Scene::Create() {
		return CreateRef<Scene>();
	}
	Scene::Scene() {
		m_ECS = Engine::ECS::ECS::Create();
		Entity root = { m_ECS->CreateEntity(), this };
		root.CreateComponent<EntityMetaData>("Untitled Scene");
		root.CreateComponent<SceneMetaData>();

		m_Running = false;
		m_PhysicsEngine = Engine::Physics::PhysicsEngine2D::Create(m_ECS);
	}
	Scene::Scene(const std::string& name) {
		m_ECS = Engine::ECS::ECS::Create();
		Entity root = { m_ECS->CreateEntity(), this };
		root.CreateComponent<EntityMetaData>(name);
		root.CreateComponent<SceneMetaData>();

		m_Running = false;
		m_PhysicsEngine = Engine::Physics::PhysicsEngine2D::Create(m_ECS);
	}
	Scene::Scene(const Scene& other) {
		m_FileName = other.m_FileName;
		m_Running = other.m_Running;

		m_ECS = CreateRef<ECS>(*other.m_ECS.get());
		
		for (const auto& [uuid, entity] : other.m_EntityUUIDs) m_EntityUUIDs.insert({ uuid, { entity.GetID(), this } });
	
		// Update parent and child entities in metadata components
		View<EntityMetaData> view(m_ECS);
		view.ForEach([&](EntityID id, EntityMetaData& data) {
			if (data.parent) data.parent = { data.parent.GetID(), this };
			for (Entity& child : data.children.GetData()) child = { child.GetID(), this };
		});

		m_PhysicsEngine = Engine::Physics::PhysicsEngine2D::Create(m_ECS);
	}
	Scene::~Scene() { 
		std::string name = GetName();
		LOG_WARNING("Destoried Scene \"%s\"", name.c_str());
	}

	Entity Scene::GetEntity(Engine::UUID uuid) { 
		if (!m_EntityUUIDs.count(uuid)) return {};
		return m_EntityUUIDs[uuid]; 
	}
	EntityMetaData& Scene::GetRoot() { 
		return Entity(0, this).GetComponent<EntityMetaData>(); 
	}
	SceneMetaData& Scene::GetMetaData() { 
		return Entity(0, this).GetComponent<SceneMetaData>(); 
	}
	std::string& Scene::GetName() {
		return GetRoot().name;
	}

	Entity Scene::CreateEntity(const std::string& name) {
		Entity entity = { m_ECS->CreateEntity(), this };
		entity.CreateComponent<EntityMetaData>(name);
		entity.CreateComponent<Transform>();
		entity.CreateComponent<TransformMetaData>();

		Engine::UUID uuid = entity.CreateComponent<Engine::UUID>();
		m_EntityUUIDs.insert({ uuid, entity });

		EntityMetaData& root = Entity(0, this).GetMetaData();
		root.children.Create(entity.GetID(), entity);

		return entity;
	}
	Entity Scene::CreateEntity(const std::string& name, Engine::UUID uuid) {
		Entity entity = { m_ECS->CreateEntity(), this };
		entity.CreateComponent<EntityMetaData>(name);
		entity.CreateComponent<Transform>();
		entity.CreateComponent<TransformMetaData>();

		entity.CreateComponent<Engine::UUID>(uuid);
		m_EntityUUIDs.insert({ uuid, entity });

		EntityMetaData& root = Entity(0, this).GetMetaData();
		root.children.Create(entity.GetID(), entity);

		return entity;
	}
	Entity Scene::CreateEntity(Entity parent, const std::string& name) {
		// Create child entity
		Entity entity = { m_ECS->CreateEntity(), this };
		entity.CreateComponent<EntityMetaData>(name, parent);

		Engine::UUID uuid = entity.CreateComponent<Engine::UUID>();
		m_EntityUUIDs.insert({ uuid, entity });

		// Create and set transform
		entity.CreateComponent<TransformMetaData>();
		Transform& transform = entity.CreateComponent<Transform>();
		transform.parent = parent.GetID();

		// Update parent entity
		EntityMetaData& metaData = parent.GetMetaData();
		metaData.children.Create(entity.GetID(), entity);

		return entity;
	}
	Entity Scene::CreateEntity(Entity parent, const std::string& name, Engine::UUID uuid) {
		// Create child entity
		Entity entity = { m_ECS->CreateEntity(), this };
		entity.CreateComponent<EntityMetaData>(name, parent);

		entity.CreateComponent<Engine::UUID>(uuid);
		m_EntityUUIDs.insert({ uuid, entity });

		// Create and set transform
		entity.CreateComponent<TransformMetaData>();
		Transform& transform = entity.CreateComponent<Transform>();
		transform.parent = parent.GetID();

		// Update parent entity
		EntityMetaData& metaData = parent.GetMetaData();
		metaData.children.Create(entity.GetID(), entity);

		return entity;
	}
	void Scene::RemoveEntity(Entity entity) {
		if (!entity) return;

		// update entity and parent entity metadata
		EntityMetaData& entityMetaData = entity.GetMetaData();
		if (entityMetaData.parent) {
			EntityMetaData& parentMetaData = entityMetaData.parent.GetMetaData();
			parentMetaData.children.Remove(entity.GetID());
		}
		else {
			EntityMetaData& root = Entity(0, this).GetMetaData();
			root.children.Remove(entity.GetID());
		}

		// Remove child entities
		for (Entity& child : entityMetaData.children.GetData()) {
			RemoveEntity(child);
		}

		// Remove uuid from map
		m_EntityUUIDs.erase(entity.GetUUID());

		m_ECS->RemoveEntity(entity.GetID());
	}

	void Scene::ChangeParent(Entity entity, Entity parent) {
		EntityMetaData& entityMetaData = entity.GetMetaData();

		// Make sure the new parent isn't the same as the old one
		if (entityMetaData.parent == parent) return;

		// Update old parent metadata
		if (entityMetaData.parent) {
			EntityMetaData& originalParent = entityMetaData.parent.GetMetaData();
			originalParent.children.Remove(entity.GetID());
		}
		else {
			EntityMetaData& root = Entity(0, this).GetMetaData();
			root.children.Remove(entity.GetID());
		}

		// Update new parent metadata
		if (parent) {
			EntityMetaData& parentMetaData = parent.GetMetaData();
			parentMetaData.children.Create(entity.GetID(), entity);
		}
		else {
			EntityMetaData& root = Entity(0, this).GetMetaData();
			root.children.Create(entity.GetID(), entity);
		}

		// Update entity metadata and transform
		entityMetaData.parent = parent;
		Transform& transform = entity.GetComponent<Transform>();
		transform.parent = parent.GetID();
	}
	bool Scene::IsChild(Entity entity, Entity parent) {
		if (!entity || !parent) return false;

		EntityMetaData& metaData = entity.GetComponent<EntityMetaData>();
		if (metaData.parent == parent) return true;

		return IsChild(metaData.parent, parent);
	}

	void Scene::StartRuntime() {
		m_Running = true;
	}
	void Scene::EndRuntime() {
		m_Running = false;
	}
	void Scene::Update(Engine::DeltaTime deltaTime) {
		if (!m_Running) return;

		m_PhysicsEngine->Update(deltaTime);
	}

	void Scene::ForEach(std::function<void(Entity)> func) {
		View<Transform> entities(m_ECS);
		entities.ForEach([&](EntityID id, Transform& transform) {
			func({ id, this });
		});
	}
	void Scene::ForEach(std::function<void(Entity, EntityMetaData&)> func) {
		View<EntityMetaData> entities(m_ECS);
		entities.ForEach([&](EntityID id, EntityMetaData& metaData) {
			func({ id, this }, metaData);
		});
	}

	SceneInspectorContext::SceneInspectorContext(ref<Scene> scene) : m_Scene(scene) { }
	static void UndoRedoSceneMetaData(ref<Scene> scene, SceneMetaData metaData) {
		scene->GetMetaData() = metaData;
	}
	static void SceneMetaDataAction(ref<Scene> scene, SceneMetaData oldMetaData) {
		Action action;
		action.undoFunc = std::bind(UndoRedoSceneMetaData, scene, oldMetaData);
		action.redoFunc = std::bind(UndoRedoSceneMetaData, scene, scene->GetMetaData());
		action.name = "Edited scene \"" + scene->GetName() + "\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}
	void SceneInspectorContext::ShowGui()
	{
		if (!m_Scene) return;
		ImGui::PushID(m_Scene->GetName().c_str());

		// Show scene name
		std::string original;
		if (EditorGui::RenamableText(m_Scene->GetName(), "SceneInspectorContextNameBox", nullptr, &original)) {
			RenameSceneAction(m_Scene, m_Scene->GetName(), original);
		}
		ImGui::Separator();

		EditorGui::Text("Entity Count", std::to_string(m_Scene->GetECS()->GetEntityCount()));

		bool edited = false;
		SceneMetaData oldMetaData = m_Scene->GetMetaData();
		if (EditorGui::InputEntity<Engine::ECS::OrthographicCamera>("Main Camera", m_Scene->GetMetaData().activeCamera, m_Scene)) edited = true;

		if (edited) SceneMetaDataAction(m_Scene, oldMetaData);
	}
}