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
			for (auto& [type, id] : entity->m_Components) {
				ECS::Component::GetGlobal(id)->Delete();
			}
			s_Entities.Delete(entity->GetID());
		}
		Entity* Entity::Get(Core::ID id)
		{
			return s_Entities.Get(id);
		}
		bool Entity::IsValid(Core::ID id)
		{
			return s_Entities.IsValid(id);
		}
	}
}