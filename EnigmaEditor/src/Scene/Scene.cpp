#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "EditorImGui.h"
#include "UndoRedoAction.h"
#include <Enigma/Engine/ECS/Components.h>

#include <imgui.h>
#include <imgui_internal.h>

using namespace Enigma::Engine::ECS;

namespace Enigma::Editor {

	ref<Scene> Scene::Create()
	{
		return CreateRef<Scene>();
	}
	Scene::Scene()
	{
		m_ECS = Engine::ECS::ECS::Create();
		Entity root = { m_ECS->CreateEntity(), this };
		root.CreateComponent<EntityMetaData>("Untitled Scene");
		root.CreateComponent<SceneMetaData>();
	}

	Scene::Scene(const std::string& name)
	{
		m_ECS = Engine::ECS::ECS::Create();
		Entity root = { m_ECS->CreateEntity(), this };
		root.CreateComponent<EntityMetaData>(name);
		root.CreateComponent<SceneMetaData>();
	}

	Scene::~Scene()
	{
		//LOG_WARNING("Destroying scene %s", GetName().c_str());
	}

	Entity Scene::GetEntity(Engine::UUID uuid) { 
		if (!m_EntityUUIDs.count(uuid)) return {};
		return m_EntityUUIDs[uuid]; 
	}
	EntityMetaData&       Scene::GetRoot()     { return Entity(0, this).GetComponent<EntityMetaData>(); }
	SceneMetaData&        Scene::GetMetaData() { return Entity(0, this).GetComponent<SceneMetaData>(); }
	ref<Engine::ECS::ECS> Scene::GetECS()      { return m_ECS; }
	std::string&          Scene::GetName()     { return GetRoot().name; }
	std::string&          Scene::GetFileName() { return m_FileName; }

	Entity Scene::CreateEntity(const std::string& name)
	{
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
	Entity Scene::CreateEntity(const std::string& name, Engine::UUID uuid)
	{
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
	Entity Scene::CreateEntity(Entity parent, const std::string& name)
	{
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

	Entity Scene::CreateEntity(Entity parent, const std::string& name, Engine::UUID uuid)
	{
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

	void Scene::ChangeParent(Entity entity, Entity parent)
	{
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

	bool Scene::IsChild(Entity entity, Entity parent)
	{
		if (!entity || !parent) return false;

		EntityMetaData& metaData = entity.GetComponent<EntityMetaData>();
		if (metaData.parent == parent) return true;

		return IsChild(metaData.parent, parent);
	}

	void Scene::RemoveEntity(Entity entity)
	{
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

	void Scene::ForEach(std::function<void(Entity)> func)
	{
		View<Transform> entities(m_ECS);
		entities.ForEach([&](EntityID id, Transform& transform) {
			func({ id, this });
		});
	}
	void Scene::ForEach(std::function<void(Entity, EntityMetaData&)> func)
	{
		View<EntityMetaData> entities(m_ECS);
		entities.ForEach([&](EntityID id, EntityMetaData& metaData) {
			func({ id, this }, metaData);
		});
	}

	void Scene::Update(Engine::DeltaTime deltaTime)
	{

	}

	SceneInspectorContext::SceneInspectorContext(ref<Scene> scene) : m_Scene(scene)
	{
		
	}
	void SceneInspectorContext::ShowGui()
	{
		if (!m_Scene) return;
		ImGui::PushID(m_Scene->GetName().c_str());

		std::string original;
		if (EditorGui::RenamableText(m_Scene->GetName(), "SceneInspectorContextNameBox", nullptr, &original)) {
			RenameSceneAction(m_Scene, m_Scene->GetName(), original);
		}

		ImGui::Separator();

		EditorGui::Text("Entity Count", std::to_string(m_Scene->GetECS()->GetEntityCount()));
		ImGui::BeginColumns("##Columns_SceneInspectorContextMainCamera", 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, 100);
		ImGui::Text("Main Camera");
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		std::vector<Entity> cameraEntities;
		i32 selection = -1;
		u32 cameraCount = 0;

		Engine::ECS::View<EntityMetaData, Engine::ECS::OrthographicCamera> cameraEntityView(m_Scene->GetECS());
		cameraEntityView.ForEach([&](EntityID id, EntityMetaData& metaData, Engine::ECS::OrthographicCamera& camera) {
			cameraEntities.push_back({ id, m_Scene.get() });
			if (id == m_Scene->GetMetaData().activeCamera.GetID()) selection = cameraCount;
			cameraCount += 1;
		});

		if (ImGui::Combo("##ComboBox", &selection, [](void* user_data, int idx) {
			std::vector<Entity>* entities = static_cast<std::vector<Entity>*>(user_data);
			Entity entity = entities->at(idx);
			return entity.GetComponent<EntityMetaData>().name.c_str();
			}, static_cast<void*>(&cameraEntities), cameraEntities.size())) {
			m_Scene->GetMetaData().activeCamera = cameraEntities[selection];
		}

		ImGui::PopStyleVar();

		ImGui::EndColumns();
		ImGui::PopID();
	}
}