#include "Engine/ECS/EntityComponentSystem.h"

#include "Engine/ECS/StanderedComponents.h"

namespace Enigma {
	namespace Engine {
		namespace ECS {
			Entity* ECS::CreateEntityImpl()
			{
				// Create entity
				Entity* entity = new Entity();
				entity->m_ID = m_Entities.Create(entity);

				// Create transform
				entity->AddComponent<Transform>(new Transform());

				return entity;
			}

			Entity* ECS::GetEntityImpl(Core::ID id)
			{
				return m_Entities.Get(id);
			}

		}
	}
}