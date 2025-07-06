#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/Component.h>
#include <Enigma/Core/Core.h>

#include "Scene/Entity.h"

namespace Enigma {
	namespace Editor {

		class Scene {
		public:
			Scene();
			~Scene();

			Engine::ECS::ECS* GetECS() { return m_ECS; }

			Core::ID CreateEntity(const std::string& name);
			Core::ID CreateEntity(const std::string& name, Core::ID parent);

			void DeleteEntity(Core::ID entity);

			std::vector<Entity*>& GetEntities();
			Entity* GetEntity(Core::ID entityID);

		private:
			Engine::ECS::ECS* m_ECS;
			Core::IDHandler<Entity*> m_EntityTree;
		};

	}
}