#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/Component.h>
#include <Enigma/Core/Core.h>

#include "Scene/Entity.h"
#include "Serialization/JSON.h"

namespace Enigma {
	namespace Editor {

		class Scene {
		public:
			Scene();
			Scene(const std::string& scenePath);
			~Scene();

			void Save();
			std::string GetPath() { return m_ScenePath; }

			Engine::ECS::ECS* GetECS() { return m_ECS; }

			template<typename T>
			EditorComponentInterface* AddComponent(Core::ID entity) {
				if (!m_EntityTree.IsValid(entity)) {
					LOG_WARNING("Failed to add component to entity, entity ID is invalid $s", ((std::string)entity).c_str());
					return nullptr;
				}
				Entity* e = m_EntityTree.Get(entity);
				Core::ID id = m_Components.Create(new T(e));
				e->compoentIDs.insert({ T::GetStaticType(), id });
				return m_Components.Get(id);
			}
			template<typename T>
			T* GetComponent(Core::ID entity) {
				if (!m_EntityTree.IsValid(entity)) {
					LOG_WARNING("Failed to get component from entity, entity ID is invalid $s", ((std::string)entity).c_str());
					return nullptr;
				}
				Entity* e = m_EntityTree.Get(entity);
				if (!e->compoentIDs.count(T::GetStaticType())) {
					LOG_WARNING("Failed to get component from entity %s, entity doesn't have component type %s", e->name.c_str(), T::GetStaticName());
					return nullptr;
				}
				if (!m_Components.IsValid(e->compoentIDs[T::GetStaticType()])) {
					LOG_WARNING("Failed to get component from entity %s, invalid component ID %s", e->name.c_str(), ((std::string)e->compoentIDs[T::GetStaticType()]).c_str());
					return nullptr;
				}
				return (T*)m_Components.Get(e->compoentIDs[T::GetStaticType()]);
			}
			EditorComponentInterface* GetComponent(Core::ID component);

			Core::ID CreateEntity(const std::string& name);
			Core::ID CreateEntity(const std::string& name, Core::ID parent);
			void SetEntityParent(Core::ID entity, Core::ID newParent);

			void DeleteEntity(Core::ID entity);

			std::vector<Entity*>& GetEntities();
			Entity* GetEntity(Core::ID entityID);
			std::string& GetName() { return m_Name; }

		private:
			void LoadComponent(JSON::DataTreeNode& componentData, Core::ID entity);
			void LoadEntity(JSON::DataTreeNode& entityData);
			void LoadEntity(JSON::DataTreeNode& entityData, Core::ID parent);

			void SaveEntity(JSON::DataTreeNode& dataTree, Core::ID entity);

			void DeleteChildEntity(Core::ID entity);

		private:
			std::string m_Name;
			std::string m_ScenePath;
			JSON::DataTreeNode m_SceneData;

			Engine::ECS::ECS* m_ECS;
			Core::IDHandler<Entity*> m_EntityTree;
			Core::IDHandler<EditorComponentInterface*> m_Components;
		};

	}
}