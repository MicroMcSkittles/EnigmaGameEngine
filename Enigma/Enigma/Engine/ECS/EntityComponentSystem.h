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

				~ECS() {
					m_Entities.Clear();
					for (auto& [type, ipool] : m_Pools) {
						delete ipool;
					}
					m_Pools.clear();
				}

				static void MakeCurrent(ECS* ecs) { s_CurrentInstance = ecs; }

				static Core::ID CreateEntity();

				template<typename T>
				static ComponentPool<T>* GetPool() {
					LOG_ASSERT(!s_CurrentInstance->m_Pools.count(T::GetType()), "No component pool of type %s exists", T::GetName());
					return (ComponentPool<T>*)s_CurrentInstance->m_Pools[T::GetType()];
				}

				template<typename T>
				static T& AddComponent(Core::ID entity) {
					LOG_ASSERT(s_CurrentInstance->m_Entities.Get(entity).components.count(T::GetType()), "Entity already has a %s component", T::GetName());
					if (!s_CurrentInstance->m_Pools.count(T::GetType()))
						s_CurrentInstance->m_Pools.insert({ T::GetType(), new ComponentPool<T>() });
					T comp;
					comp.parentEntity = entity;
					Core::ID compID = ((ComponentPool<T>*)s_CurrentInstance->m_Pools[T::GetType()])->Add(comp);
					s_CurrentInstance->m_Entities.Get(entity).components.insert({ T::GetType(), compID });
					return ((ComponentPool<T>*)s_CurrentInstance->m_Pools[T::GetType()])->Get(compID);
				}

				template<typename T>
				static T& GetComponent(Core::ID entity) {
					ComponentPool<T>* pool = GetPool<T>();
					LOG_ASSERT(!s_CurrentInstance->m_Entities.Get(entity).components.count(T::GetType()), "Entity does not have a %s component", T::GetName());
					return pool->Get(s_CurrentInstance->m_Entities.Get(entity).components[T::GetType()]);
				}

			private:
				Core::IDHandler<Entity> m_Entities;
				std::map<ComponentType, ComponentPoolInterface*> m_Pools;

			private:
				inline static ECS* s_CurrentInstance;
			};

		}
	}
}