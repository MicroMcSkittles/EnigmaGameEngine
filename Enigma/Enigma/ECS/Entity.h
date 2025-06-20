#pragma once
#include "Core/IdHandler.h"
#include "ECS/Component.h"

#include <map>

namespace Enigma {
	namespace ECS {

		class Entity {
		public:
			static Entity* Create();
			static void Delete(Entity* entity);
			static Entity* Get(Core::ID id);
			static bool IsValid(Core::ID id);
			Core::ID GetID() { return m_ID; }

			template<typename T>
			void AddComponent(T* component) {
				if (m_Components.count(T::StaticGetType())) {
					LOG_WARNING("Entity with id " + std::string(m_ID) + " already has " + T::StaticGetName() + " component");
					return;
				}
				component->SetParentID(m_ID);
				m_Components.insert({ T::StaticGetType(), component->GetGlobalID() });
			}

			template<typename T>
			T* GetComponent() {
				return T::GetGlobal(m_Components[T::StaticGetType()]);
			}

			template<typename T>
			bool HasComponent() {
				return m_Components.count(T::StaticGetType());
			}

			std::string& GetTag() { return m_Tag; }

		private:
			Entity() { 
				m_ID = Core::ID::InvalidID();
			}

		private:
			Core::ID m_ID;
			std::map<ComponentType, Core::ID> m_Components;
			std::string m_Tag;

		private:
			inline static Core::IDHandler<Entity> s_Entities;
		};

	}
}