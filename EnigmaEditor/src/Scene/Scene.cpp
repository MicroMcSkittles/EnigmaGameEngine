#include "Scene/Scene.h"
#include "Scene/Component.h"

namespace Enigma {
	namespace Editor {

		Scene::Scene() {
			m_ECS = new Engine::ECS::ECS();
		}

		void Scene::LoadComponent(JSON::DataTreeNode& componentData, Core::ID entity)
		{
			using namespace Engine::ECS;
			std::string type = componentData["Type"].value;
			if      (type == "Tag")       AddComponent<EditorTag>(entity)->Load(componentData);
			else if (type == "Transform") AddComponent<EditorTransform>(entity)->Load(componentData);
			else if (type == "Render2D")  AddComponent<EditorRender2D>(entity)->Load(componentData);
			else LOG_WARNING("Unknown component type");
		}
		void Scene::LoadEntity(JSON::DataTreeNode& entityData)
		{
			using namespace Engine::ECS;
			ECS::MakeCurrent(m_ECS);
			std::string name = entityData["Name"].value;
			Core::ID id = CreateEntity(name);

			for (JSON::DataTreeNode& componentData : entityData["Components"].elements) {
				LoadComponent(componentData, id);
			}

			for (JSON::DataTreeNode& childData : entityData["Children"].elements) {
				LoadEntity(childData, id);
			}
		}
		void Scene::LoadEntity(JSON::DataTreeNode& entityData, Core::ID parent)
		{
			using namespace Engine::ECS;
			ECS::MakeCurrent(m_ECS);
			std::string name = entityData["Name"].value;
			Core::ID id = CreateEntity(name, parent);

			for (JSON::DataTreeNode& componentData : entityData["Components"].elements) {
				LoadComponent(componentData, id);
			}

			for (JSON::DataTreeNode& childData : entityData["Children"].elements) {
				LoadEntity(childData, id);
			}
		}
	
		Scene::Scene(const std::string& scenePath)
		{
			m_ScenePath = scenePath;
			m_ECS = new Engine::ECS::ECS();
			JSON::JSON::LoadFile(m_ScenePath, &m_SceneData);

			m_Name = m_SceneData["Name"].value;

			for (JSON::DataTreeNode& entityData : m_SceneData["Entities"].elements) {
				LoadEntity(entityData);
			}
		}

		void Scene::SaveEntity(JSON::DataTreeNode& dataTree, Core::ID entity)
		{
			JSON::DataTreeNode node;
			node.value.type = JSON::DataTreeType::Object;
			node.format.childOrder = { "Name", "Components", "Children" };

			Entity* e = m_EntityTree.Get(entity);

			node["Name"] = e->name;

			{
				using namespace Engine::ECS;
				ECS::MakeCurrent(m_ECS);
				JSON::DataTreeNode components;
				components.value.type = JSON::DataTreeType::Array;

				for (auto& [type, id] : ECS::GetEntity(e->entityID).components) {
					JSON::DataTreeNode component;
					switch (type)
					{
					case ComponentType::Tag:       GetComponent<EditorTag>(entity)->Save(component); break;
					case ComponentType::Transform: GetComponent<EditorTransform>(entity)->Save(component); break;
					case ComponentType::Render2D:  GetComponent<EditorRender2D>(entity)->Save(component); break;
					default: continue;
					}
					components.elements.push_back(component);
				}

				node["Components"] = components;
			}

			{
				JSON::DataTreeNode children;
				children.value.type = JSON::DataTreeType::Array;
				for (Core::ID childID : e->childrenIDs) {
					SaveEntity(children, childID);
				}
				node["Children"] = children;
			}

			dataTree.elements.push_back(node);
		}
		void Scene::Save()
		{
			if (m_ScenePath.empty()) return;
			JSON::DataTreeNode save;
			save.value.type = JSON::DataTreeType::Object;
			save.format.childOrder = { "Name", "Entities" };
			save["Name"] = m_Name;

			JSON::DataTreeNode entities;
			entities.value.type = JSON::DataTreeType::Array;
			for (Entity* entity : m_EntityTree.GetData()) {
				if (m_EntityTree.IsValid(entity->parentID)) continue;
				SaveEntity(entities, entity->ID);
			}
			save["Entities"] = entities;

			JSON::JSON::Serialize(m_ScenePath, &save);
		}
		Scene::~Scene()
		{
			Save();

			for (Entity* entity : m_EntityTree.GetData()) {
				delete entity;
			}
			m_EntityTree.Clear();
			delete m_ECS;
		}

		EditorComponentInterface* Scene::GetComponent(Core::ID component) {
			if (!m_Components.IsValid(component)) {
				LOG_WARNING("Failed to get component, invalid ID %s", ((std::string)component).c_str());
				return nullptr;
			}
			return m_Components.Get(component);
		}

		Core::ID Scene::CreateEntity(const std::string& name) {
			Engine::ECS::ECS::MakeCurrent(m_ECS);

			Entity* e = new Entity();
			e->name = name;
			e->scene = this;
			
			e->entityID = Engine::ECS::ECS::CreateEntity();
			e->parentID = Core::ID::InvalidID();
			e->ID = m_EntityTree.Create(e);

			return e->ID;
		}
		Core::ID Scene::CreateEntity(const std::string& name, Core::ID parent) {
			Engine::ECS::ECS::MakeCurrent(m_ECS);

			Entity* e = new Entity();
			e->name = name;
			e->scene = this;

			e->entityID = Engine::ECS::ECS::CreateEntity();
			e->parentID = Core::ID::InvalidID();
			e->ID = m_EntityTree.Create(e);

			if (!m_EntityTree.IsValid(parent)) {
				LOG_WARNING("Entity parent ID ( %s ) is not valid", ((std::string)parent).c_str());
				return e->ID;
			}

			e->parentID = parent;
			m_EntityTree.Get(parent)->childrenIDs.push_back(e->ID);

			Engine::ECS::Transform& transform = Engine::ECS::ECS::GetComponent<Engine::ECS::Transform>(e->entityID);
			transform.parentTransform = Engine::ECS::ECS::GetComponentID<Engine::ECS::Transform>(m_EntityTree.Get(parent)->entityID);

			return e->ID;
		}
		void Scene::SetEntityParent(Core::ID entity, Core::ID newParent)
		{
			// Make sure entity exists in the entity tree
			if (!m_EntityTree.IsValid(entity)) {
				LOG_WARNING("Failed to set parent, ID is invalid %s", ((std::string)entity).c_str());
				return;
			}
			Entity* e = m_EntityTree.Get(entity);

			// If entity had a parent then remove entity from the parents children ids
			if (m_EntityTree.IsValid(e->parentID)) {
				Entity* parent = m_EntityTree.Get(e->parentID);
				// Loop through childrenIDs to remove entity
				for (int i = 0; i < parent->childrenIDs.size(); ++i) {
					Core::ID& childID = parent->childrenIDs[i];
					if (childID == entity) {
						parent->childrenIDs.erase(parent->childrenIDs.begin() + i);
						break;
					}
				}
			}

			using namespace Engine::ECS;
			ECS::MakeCurrent(m_ECS);

			// if newParent is equal to InvalidID then entity has no parent
			if (newParent == Core::ID::InvalidID()) {
				e->parentID = Core::ID::InvalidID();
				Transform& transform = ECS::GetComponent<Transform>(e->entityID);
				transform.parentTransform = Core::ID::InvalidID();
				return;
			}

			// Make sure the new parent exists in the entity tree
			if (!m_EntityTree.IsValid(newParent)) {
				LOG_WARNING("Failed to set parent for %s, parent ID is invalid %s", e->name.c_str(), ((std::string)newParent).c_str());
				return;
			}
			Entity* parent = m_EntityTree.Get(newParent);
			parent->childrenIDs.push_back(entity);

			// Set the parent transform of the entity's transform
			Transform& transform = ECS::GetComponent<Transform>(e->entityID);
			transform.parentTransform = ECS::GetComponentID<Transform>(parent->entityID);

			e->parentID = newParent;
		}

		void Scene::DeleteEntity(Core::ID entity)
		{
			if (!m_EntityTree.IsValid(entity)) {
				LOG_WARNING("ID isn't valid ( %s )", ((std::string)entity).c_str());
				return;
			}

			Entity* e = m_EntityTree.Get(entity);
			
			if (m_EntityTree.IsValid(e->parentID)) {
				Entity* parent = m_EntityTree.Get(e->parentID);
				for (int i = 0; i < parent->childrenIDs.size(); ++i) {
					if (m_EntityTree.Get(parent->childrenIDs[i])->ID == e->ID) {
						parent->childrenIDs.erase(parent->childrenIDs.begin() + i);
						break;
					}
				}
			}
			
			for (Core::ID& childID : e->childrenIDs) {
				DeleteChildEntity(childID);
			}

			m_ECS->DeleteEntity(e->entityID);
			m_EntityTree.Delete(entity);
			delete e;
		}
		void Scene::DeleteChildEntity(Core::ID entity)
		{
			if (!m_EntityTree.IsValid(entity)) {
				LOG_WARNING("ID isn't valid ( %s )", ((std::string)entity).c_str());
				return;
			}

			Entity* e = m_EntityTree.Get(entity);

			for (Core::ID& childID : e->childrenIDs) {
				DeleteEntity(childID);
			}

			m_ECS->DeleteEntity(e->entityID);
			m_EntityTree.Delete(entity);
			delete e;
		}

		std::vector<Entity*>& Scene::GetEntities() { 
			return m_EntityTree.GetData(); 
		}
		Entity* Scene::GetEntity(Core::ID entityID) {
			return m_EntityTree.Get(entityID);
		}
	}
}