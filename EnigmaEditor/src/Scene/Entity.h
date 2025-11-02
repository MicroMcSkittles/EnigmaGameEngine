#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>

#include "Scene/Scene.h"

namespace Enigma::Editor {

	// Wrapper for the ECS entity
	class Entity {
	public:
		Entity(const Entity& other) = default;
		Entity() : m_EntityID(Engine::ECS::InvalidEntityID), m_Scene(nullptr) { }
		Entity(Engine::ECS::EntityID entityID, Scene* scene) : m_EntityID(entityID), m_Scene(scene) { }

		Engine::ECS::EntityID GetID() { return m_EntityID; }
		bool Valid() { return (m_EntityID != Engine::ECS::InvalidEntityID) && (m_Scene != nullptr); }

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
		bool HasComponent() {
			return m_Scene->m_ECS->HasComponent<T>(m_EntityID);
		}

		bool operator ==(const Entity& other) {
			return (m_EntityID == other.m_EntityID) && (m_Scene == other.m_Scene);
		}
		bool operator !=(const Entity& other) {
			return (m_EntityID != other.m_EntityID) && (m_Scene != other.m_Scene);
		}

	private:
		Engine::ECS::EntityID m_EntityID;
		Scene* m_Scene;
	};

}