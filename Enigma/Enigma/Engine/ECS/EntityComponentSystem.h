#pragma once
#include "Engine/ECS/Component.h"
#include "Core/IdHandler.h"
#include "Core/Core.h"
#include <map>

// Times writen: 3
// I'm going insaine
// I am like a room full of monkies. I am destined to type for a infinite amount
// of time to get any kind of result with this system

namespace Enigma {
	namespace Engine {
		namespace ECS {

			struct Entity {
				std::map<ComponentType, Core::ID> components;
			};

			class ECS {
			public:

				Core::ID CreateEntity();

				template<typename T>
				ComponentPool<T>* GetPool() {
					LOG_ASSERT(!m_Pools.count(typename T::GetType()), "No component pool of type %s exists", typename T::GetName());
					return (ComponentPool<T>*)m_Pools[typename T::GetType()];
				}
				template<typename T>
				Core::ID AddComponent(Core::ID entity) {
					LOG_ASSERT(m_Entities.Get(entity).components.count(typename T::GetType()), "Entity already has a %s component", typename T::GetName());
					if (!m_Pools.count(typename T::GetType()))
						m_Pools.insert({ typename T::GetType(), new ComponentPool<T>() });
					T comp;
					comp.parentEntity = entity;
					Core::ID compID = ((ComponentPool<T>*)m_Pools[typename T::GetType()])->Add(comp);
					m_Entities.Get(entity).components.insert({ typename T::GetType(), compID });
					return compID;
				}
				template<typename T>
				T& GetComponent(Core::ID entity) {
					ComponentPool<T>* pool = GetPool<T>();
					LOG_ASSERT(!m_Entities.Get(entity).components.count(typename T::GetType()), "Entity does not have a %s component", typename T::GetName());
					return pool->Get(m_Entities.Get(entity).components[typename T::GetType()]);
				}

			private:
				Core::IDHandler<Entity> m_Entities;
				std::map<ComponentType, IComponentPool*> m_Pools;
			};

		}
	}
}