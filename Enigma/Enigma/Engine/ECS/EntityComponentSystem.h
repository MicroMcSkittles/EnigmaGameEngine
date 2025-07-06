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

					// Delete component pools
					for (auto& [type, ipool] : m_Pools) {
						delete ipool;
					}
					m_Pools.clear();
				}

				static void MakeCurrent(ECS* ecs) { s_CurrentInstance = ecs; }

				static Core::ID CreateEntity();
				static void DeleteEntity(Core::ID id);

				static Entity& GetEntity(Core::ID id);

				template<typename T>
				static ComponentPool<T>* GetPool() {
					// Make sure a Component Pool of T type exists
					LOG_ASSERT(!s_CurrentInstance->m_Pools.count(T::GetType()), "No component pool of type %s exists", T::GetName());
					// Return the pool
					return (ComponentPool<T>*)s_CurrentInstance->m_Pools[T::GetType()];
				}

				template<typename T>
				static T& AddComponent(Core::ID entity) {
					// Make sure the entity does not have the component T
					LOG_ASSERT(s_CurrentInstance->m_Entities.Get(entity).components.count(T::GetType()), "Entity already has a %s component", T::GetName());
					
					// If a pool of T type does not exist then create one
					if (!s_CurrentInstance->m_Pools.count(T::GetType()))
						s_CurrentInstance->m_Pools.insert({ T::GetType(), new ComponentPool<T>() });
					
					// Create component
					T comp;
					comp.parentEntity = entity;
					Core::ID compID = ((ComponentPool<T>*)s_CurrentInstance->m_Pools[T::GetType()])->Add(comp);
					
					// Store the components ID in the entity
					s_CurrentInstance->m_Entities.Get(entity).components.insert({ T::GetType(), compID });
					
					// Return a reference to the component
					return ((ComponentPool<T>*)s_CurrentInstance->m_Pools[T::GetType()])->Get(compID);
				}

				template<typename T>
				static void RemoveComponent(Core::ID entity) {
					// Make sure entity has a component T
					LOG_ASSERT(!s_CurrentInstance->m_Entities.Get(entity).components.count(T::GetType()), "Entity doesn't have a %s component", T::GetName());
					Entity& e = s_CurrentInstance->m_Entities.Get(entity);
					ComponentPool<T>* pool = GetPool<T>();
					// Remove the component
					pool->Remove(e.components[T::GetType()]);
				}

				template<typename T>
				static T& GetComponent(Core::ID entity) {
					// Make sure entity has component T
					LOG_ASSERT(!s_CurrentInstance->m_Entities.Get(entity).components.count(T::GetType()), "Entity does not have a %s component", T::GetName());
					ComponentPool<T>* pool = GetPool<T>();
					// Return a reference to the component
					return pool->Get(s_CurrentInstance->m_Entities.Get(entity).components[T::GetType()]);
				}

				template<typename T>
				static Core::ID GetComponentID(Core::ID entity) {
					// Make sure entity has component T
					LOG_ASSERT(!s_CurrentInstance->m_Entities.Get(entity).components.count(T::GetType()), "Entity does not have a %s component", T::GetName());
					// return the components ID
					return s_CurrentInstance->m_Entities.Get(entity).components[T::GetType()];
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