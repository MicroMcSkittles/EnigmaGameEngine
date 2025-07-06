#include "Scene/Scene.h"

namespace Enigma {
	namespace Editor {

		Scene::Scene() {
			m_ECS = new Engine::ECS::ECS();
		}

		Scene::~Scene()
		{
			for (Entity* entity : m_EntityTree.GetData()) {
				delete entity;
			}
			m_EntityTree.Clear();
			delete m_ECS;
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