#include "Enigma/EnigmaPCH.h"
#include "Enigma/Engine/ECS/EntityComponentSystem.h"

namespace Enigma::Engine::ECS {
	// Constructors/Deconstructor
	ref<ECS> ECS::Create() { return CreateRef<ECS>(); }
	ECS::ECS() { 
		m_EntityCount = 0;
	}
	ECS::ECS(const ECS& other) { 
		m_EntityComponentMasks = other.m_EntityComponentMasks;
		m_AvailableEntityIDs = other.m_AvailableEntityIDs;
		m_EntityCount = other.m_EntityCount;

		//m_ComponentPoolMaskBits = other.m_ComponentPoolMaskBits;
		//m_EntityGroups = other.m_EntityGroups;
		for (const auto& [hash, bit] : other.m_ComponentPoolMaskBits) m_ComponentPoolMaskBits.insert({ hash, bit });
		for (const auto& [hash, group] : other.m_EntityGroups) m_EntityGroups.insert({ hash, group });
		for (const auto& [hash, pool] : other.m_ComponentPools) m_ComponentPools.insert({ hash, pool->Clone() });
	}
	ECS::~ECS() { }

	EntityID ECS::CreateEntity() {
		EntityID availableID = InvalidEntityID;

		// If there is a available entity id than take it and remove it from the m_AvailableEntityIDs
		if (!m_AvailableEntityIDs.empty()) {
			for (u64 i = 0; i < m_AvailableEntityIDs.size(); ++i) {
				if (m_AvailableEntityIDs[i] < m_EntityCount + 1) {
					availableID = m_AvailableEntityIDs[i];
					m_AvailableEntityIDs.erase(m_AvailableEntityIDs.begin() + i);
					break;
				}
			}
		}
		// If there where no available entity ids then create a new one
		else availableID = m_EntityCount;
		m_EntityCount += 1;

		// Create a component mask for the new entity
		m_EntityComponentMasks.Create(availableID);

		return availableID;
	}
	void ECS::RemoveEntity(EntityID entityID) {
		// Remove all the components that made up entity id
		for (auto& [type, pool] : m_ComponentPools) {
			if (!pool->Contains(entityID)) continue;
			pool->Remove(entityID);
		}

		// Remove the entity id from the group
		m_EntityGroups[m_EntityComponentMasks.Get(entityID)].Remove(entityID);
		// Remove the mask
		m_EntityComponentMasks.Remove(entityID);

		// Add the id to the available ids so it can be reused
		m_EntityCount -= 1;
		if (entityID == m_EntityCount) return;
		m_AvailableEntityIDs.push_back(entityID);
	}

	// Getters
	u64 ECS::GetEntityCount() { return m_EntityCount; }
}