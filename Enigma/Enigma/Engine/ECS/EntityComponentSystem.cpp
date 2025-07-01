#include "Engine/ECS/EntityComponentSystem.h"

namespace Enigma {
	namespace Engine {
		namespace ECS {
			Core::ID ECS::CreateEntity()
			{
				Core::ID id = m_Entities.Create(Entity());
				AddComponent<Transform>(id);
				return id;
			}
		}
	}
}