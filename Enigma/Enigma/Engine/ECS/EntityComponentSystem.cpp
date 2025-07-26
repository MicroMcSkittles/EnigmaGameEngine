#include "Enigma/Engine/ECS/EntityComponentSystem.h"

namespace Enigma {
	namespace Engine {
		namespace ECS {
			Core::ID ECS::CreateEntity()
			{
				Core::ID id = s_CurrentInstance->m_Entities.Create(Entity());
				AddComponent<Transform>(id);
				return id;
			}
			void ECS::DeleteEntity(Core::ID id)
			{
				Entity& entity = GetEntity(id);
				for (auto& [type, compID] : entity.components) {
					s_CurrentInstance->m_Pools[type]->Remove(compID);
				}
				s_CurrentInstance->m_Entities.Delete(id);
			}
			Entity& ECS::GetEntity(Core::ID id)
			{
				return s_CurrentInstance->m_Entities.Get(id);
			}

		}
	}
}