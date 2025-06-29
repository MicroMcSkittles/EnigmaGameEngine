#pragma once
#include "Core/IdHandler.h"
#include "Engine/ECS/Component.h"

#include <map>

// Times rewriten: 2

// To be clear, the entity component system I wrote works with hopes and dreams and nothing else.
// At this point I think it might be alive

namespace Enigma {
	namespace Engine {
		namespace ECS {

			class Entity;

			// The entity component system handler class
			class ECS {
			public:

				static void MakeCurrent(ECS* ecs) { s_ActiveECS = ecs; }
				static ECS* GetActive() { return s_ActiveECS; }

				static Entity* CreateEntity() { return s_ActiveECS->CreateEntityImpl(); }
				static Entity* GetEntity(Core::ID id) { return s_ActiveECS->GetEntityImpl(id); }

				template<typename T>
				static std::vector<T*>& GetComponentList() { return s_ActiveECS->GetComponentListImpl<T>(); }

				template<typename T>
				static T* GetComponent(Core::ID id) { return s_ActiveECS->GetComponentImpl<T>(id); }

				template<typename T>
				static Core::ID AddComponent(T* component) { return s_ActiveECS->AddComponentImpl<T>(component); }

			private:
				Entity* CreateEntityImpl();
				Entity* GetEntityImpl(Core::ID id);

				template<typename T>
				std::vector<T*>& GetComponentListImpl() {
					if (!m_Components.count(T::GetStaticType())) {
						return std::vector<T*>();
					}
					return ((T::List*)m_Components[T::GetStaticType()])->GetList();
				}

				template<typename T>
				T* GetComponentImpl(Core::ID id) {
					LOG_ASSERT(!m_Components[T::GetStaticType()]->Exists(id), "%s at %s doesn't exist.", T::GetStaticName().c_str(), ((std::string)id).c_str());
					return (T*)m_Components[T::GetStaticType()]->Get(id);
				}

				template<typename T>
				Core::ID AddComponentImpl(T* component) {
					// If component type hadn't been created before then create a list for it
					if (!m_Components.count(T::GetStaticType())) {
						m_Components.insert({ T::GetStaticType(), new T::List() });
					}
					// Create component
					Core::ID id = m_Components[T::GetStaticType()]->Create(component);
					return id;
				}

			private:
				Core::IDHandler<Entity> m_Entities;
				std::map<ComponentType, ComponentList*> m_Components;

			private:
				inline static ECS* s_ActiveECS;
			};

			class Entity {
			public:
				Core::ID GetID() { return m_ID; }

				template<typename T>
				void AddComponent(T* component) {
					LOG_ASSERT(m_Components.count(T::GetStaticType()), "Entity already has a %s", T::GetStaticName().c_str());
					component->SetParentEntityID(m_ID);
					Core::ID id = ECS::AddComponent<T>(component);
					m_Components.insert({ T::GetStaticType(), id });
				}

				template<typename T>
				T* GetComponent() {
					LOG_ASSERT(!m_Components.count(T::GetStaticType()), "Entity doesn't have a %s Component", T::GetStaticName().c_str());
					return ECS::GetComponent<T>(m_Components[T::GetStaticType()]);
				}

				std::map<ComponentType, Core::ID>& GetComponents() { return m_Components; }

			private:
				Entity() { }

			private:
				Core::ID m_ID;
				std::map<ComponentType, Core::ID> m_Components;

			private:
				friend ECS;
			};

		}
	}
}