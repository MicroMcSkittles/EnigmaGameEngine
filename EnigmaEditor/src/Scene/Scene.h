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

			Engine::ECS::ECS* GetECS() { return m_ECS; }

			Core::ID CreateEntity(const std::string& name);
			Core::ID CreateEntity(const std::string& name, Core::ID parent);

			void DeleteEntity(Core::ID entity);

			std::vector<Entity*>& GetEntities();
			Entity* GetEntity(Core::ID entityID);

		private:
			void LoadComponent(JSON::DataTreeNode& componentData, Core::ID entity);
			void LoadEntity(JSON::DataTreeNode& entityData);
			void LoadEntity(JSON::DataTreeNode& entityData, Core::ID parent);

		private:
			std::string m_Name;
			std::string m_ScenePath;
			JSON::DataTreeNode m_SceneData;

			Engine::ECS::ECS* m_ECS;
			Core::IDHandler<Entity*> m_EntityTree;
		};

	}
}