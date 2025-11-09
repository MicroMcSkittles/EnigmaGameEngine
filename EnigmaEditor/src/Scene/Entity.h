#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>

#include "Scene/Scene.h"
#include "Panels/InspectorPanel.h"

namespace Enigma::Editor {

	// Wrapper for the ECS entity
	class Entity {
	public:
		Entity();
		Entity(Engine::ECS::EntityID entityID, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T>
		T& GetComponent() {
			return m_Scene->m_ECS->GetComponent<T>(m_EntityID);
		}
		template<typename T, typename... Args>
		T& CreateComponent(Args&&... args) {
			return m_Scene->m_ECS->CreateComponent<T>(m_EntityID, std::forward<Args>(args)...);
		}
		template<typename T>
		void RemoveComponent() {
			m_Scene->m_ECS->RemoveComponent<T>(m_EntityID);
		}

		template<typename T>
		bool HasComponent() const {
			return m_Scene->m_ECS->HasComponent<T>(m_EntityID);
		}

		bool operator ==(const Entity& other) {
			return (m_EntityID == other.m_EntityID) && (m_Scene == other.m_Scene);
		}
		bool operator !=(const Entity& other) {
			return (m_EntityID != other.m_EntityID) || (m_Scene != other.m_Scene);
		}

		Engine::ECS::EntityID GetID() const { return m_EntityID; }

		// TODO: update the places valid got used
		bool Valid() const;
		operator bool() { return Valid(); }
		operator bool() const { return Valid(); }

	private:
		Engine::ECS::EntityID m_EntityID;
		Scene* m_Scene;
	};

	class EntityInspectorContext : public InspectorContext {
	public:
		static ref<EntityInspectorContext> Create(Entity entity) { return CreateRef<EntityInspectorContext>(entity); }
		EntityInspectorContext(Entity entity);

		virtual void ShowGui() override;

	private:
		Entity m_Entity;
	};
}