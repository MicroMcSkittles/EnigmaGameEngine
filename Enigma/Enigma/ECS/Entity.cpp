#include "ECS/Entity.h"

namespace Enigma {
	namespace ECS {
		Entity* Entity::Create()
		{
			Entity* entity = new Entity();
			entity->m_ID = s_Entities.Create(entity);
			entity->AddComponent<Transform>(new Transform());
			return entity;
		}
		void Entity::Delete(Entity* entity)
		{
			s_Entities.Delete(entity->GetID());
		}
		Entity* Entity::Get(Core::ID id)
		{
			return s_Entities.Get(id);
		}
	}
}