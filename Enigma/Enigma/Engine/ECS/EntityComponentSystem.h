#pragma once
#include "Enigma/Core/Core.h"
#include "Enigma/Core/SparseSet.h"
#include <tuple>
#include <algorithm>
#include <bitset>

// Times writen: 3
// I'm going insane
// I am like a room full of monkies. I am destined to type for a infinite amount
// of time to get any kind of result with this system

// Times writen: 4
// Why must I subject myself to this
// 
// It's so much better now!
//
// On my pc it can go up to 500000 entities at 60 fps on release mode

namespace Enigma::Engine::ECS {

	typedef   u64      EntityID;
	constexpr EntityID InvalidEntityID = std::numeric_limits<EntityID>::max();
	constexpr u64      ComponentPoolPageSize = 64;
	constexpr u64      MaxComponentPoolCount = 64;

	class IComponentPool {
	public:
		virtual ~IComponentPool() { }

		virtual void Create(EntityID entityID) = 0;
		virtual void Remove(EntityID entityID) = 0;
		virtual bool Contains(EntityID entityID) = 0; // returns true if the entity has a component in the component pool
		virtual u64 Size() = 0;
		virtual std::vector<u64>& GetIDs() = 0;
	};

	template<typename T>
	class ComponentPool : public IComponentPool {
	public:

		virtual void Create(EntityID entityID) override { m_Components.Create(entityID); }
		virtual void Remove(EntityID entityID) override { m_Components.Remove(entityID); }
		virtual bool Contains(EntityID entityID) override { return m_Components.Contains(entityID); }
		virtual u64 Size() override { return m_Components.Size(); }
		virtual std::vector<u64>& GetIDs() override { return m_Components.GetIDs(); }

		T& GetComponent(EntityID entityID) { return m_Components.Get(entityID); }

	private:
		Core::SparseSet<T, ComponentPoolPageSize> m_Components;
	};

	template<typename...>
	class View;

	class ECS {
	private:
		template<typename...>
		friend class View;

		// Used to generate a unique number for component types
		template<typename T>
		class ComponentHasher {
		public:
			static u64 Hash() { return reinterpret_cast<u64>(&s_ID); }
		private:
			inline static char s_ID;
		};
		
		using ComponentMask = std::bitset<MaxComponentPoolCount>;

	public:
		static ref<ECS> Create() {
			return CreateRef<ECS>();
		}
		ECS() {
			m_EntityCount = 0;
		}
		~ECS() { }

		u64 GetEntityCount() {
			return m_EntityCount;
		}

		template<typename T>
		ref<ComponentPool<T>> GetComponentPool() {
			// Make sure component pool exists
			u64 componentHash = ComponentHasher<T>::Hash();
			LOG_ASSERT(!m_ComponentPools.count(componentHash), "Failed to get component pool");

			// Get the pool
			return CastRef<ComponentPool<T>>(m_ComponentPools[componentHash]);
		}
		template<typename T>
		T& GetComponent(EntityID entityID) {
			ref<ComponentPool<T>> pool = GetComponentPool<T>();
			return pool->GetComponent(entityID);
		}

		template<typename T>
		T& CreateComponent(EntityID entityID) {
			u64 componentHash = ComponentHasher<T>::Hash();

			// If component pool for component hash doesn't exist than create one
			if (!m_ComponentPools.count(componentHash)) {
				m_ComponentPoolBits.insert({ componentHash, m_ComponentPools.size() });
				m_ComponentPools.insert({ componentHash, CreateRef<ComponentPool<T>>() });
			}

			// Update bit mask and entity groups
			ComponentMask& mask = m_EntityComponentMasks.Get(entityID);
			if (m_EntityGroups.count(mask) && m_EntityGroups[mask].Contains(entityID)) {
				m_EntityGroups[mask].Remove(entityID);
			}
			mask[m_ComponentPoolBits[componentHash]] = true;
			if (!m_EntityGroups.count(mask)) m_EntityGroups.insert({ mask, {} });
			m_EntityGroups[mask].Create(entityID, entityID);

			// Get the component pool
			ref<ComponentPool<T>> pool = CastRef<ComponentPool<T>>(m_ComponentPools[componentHash]);
			pool->Create(entityID);
			return pool->GetComponent(entityID);
		}
		template<typename T>
		void RemoveComponent(EntityID entityID) {
			u64 componentHash = ComponentHasher<T>::Hash();
			LOG_ASSERT(!m_ComponentPools.count(componentHash), "Failed to remove component");

			// Get the entity's component mask
			ComponentMask& mask = m_EntityComponentMasks.Get(entityID);

			// Move the entity to the new group
			m_EntityGroups[mask].Remove(entityID);
			if (!m_EntityGroups.count(mask)) m_EntityGroups.insert({ mask, {} });
			m_EntityGroups[mask].Create(entityID, entityID);

			// Update bit mask
			mask[m_ComponentPoolBits[componentHash]] = false;

			// Get the component pool
			ref<ComponentPool<T>> pool = CastRef<ComponentPool<T>>(m_ComponentPools[componentHash]);
			pool->Remove(entityID);
		}

		template<typename T>
		bool HasComponent(EntityID entityID) {
			u64 componentHash = ComponentHasher<T>::Hash();
			// Make sure the entity exitsts
			if (!m_EntityComponentMasks.Contains(entityID)) return false;
			// Check if component pool exists
			if (!m_ComponentPools.count(componentType)) return false;
			// Make sure the entity has that component
			if (!m_EntityComponentMasks.Get(entityID)[m_ComponentPoolBits[componentType]]) return false;
			return true;
		}

		EntityID CreateEntity() {
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
		void RemoveEntity(EntityID entityID) {
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

	private:
		std::unordered_map<u64, ref<IComponentPool>> m_ComponentPools; // Maps component hashes to component pools
		std::unordered_map<u64, u64> m_ComponentPoolBits; // Maps component hashes to a bit in a component mask

		u64 m_EntityCount; // The number of entities
		std::vector<EntityID> m_AvailableEntityIDs; // All unused entity ids
		Core::SparseSet<ComponentMask> m_EntityComponentMasks; // Component masks for each entity
		std::unordered_map<ComponentMask, Core::SparseSet<EntityID>> m_EntityGroups; // Maps a component mask to a list of entities that have that mask
	};

	template<typename... Types>
	class View {
	private:
		// Used for calling static asserts, this makes the static asserts evaluate at compile time
		template<class T>
		struct DependentFalse : std::false_type { };

		// Required for the function of the MakeGetters function
		template <typename Tuple, u64... Indices>
		auto MakeGettersImpl(Tuple& t, std::index_sequence<Indices...>) {
			return std::vector<std::function<u64()>> {
				[&t]() { return ECS::ComponentHasher<std::tuple_element_t<Indices, std::tuple<Types...>>>::Hash(); }...
			};
		}
		// Returns a list of lamda functions that each return the hash of each element in the tuple
		template <typename Tuple>
		auto MakeGetters(Tuple t) {
			return MakeGettersImpl(t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
		}

		// Returns a pointer to the component pool at Types Index
		template<u64 Index>
		auto GetPoolAt() {
			return std::static_pointer_cast<ComponentPool<std::tuple_element_t<Index, std::tuple<Types...>>>>(m_ComponentPools[Index]);
		}
		// Returns a tuple that contains the components at id from the component pools at Indices
		template<u64... Indices>
		auto MakeComponentTuple(EntityID id, std::index_sequence<Indices...>) {
			return std::make_tuple((std::ref(GetPoolAt<Indices>()->GetComponent(id)))...);
		}

		// Runs a function lamda for each entity in the view
		template<typename Func>
		void ForEachImpl(Func func) {
			if (m_Empty) return;
			constexpr auto inds = std::make_index_sequence<sizeof...(Types)>{};

			// Check if the function has the parameters (EntityID, Types&...), call it if it does
			if constexpr (std::is_invocable_v<Func, EntityID, Types&...>)
				for (EntityID& entityID : m_Smallest->GetIDs()) { std::apply(func, std::tuple_cat(std::make_tuple(entityID), MakeComponentTuple(entityID, inds))); }
			
			// Check if the function has the parameters (Types&...), call it if it does
			else if constexpr (std::is_invocable_v<Func, Types&...>) 
				for (EntityID& entityID : m_Smallest->GetIDs()) { std::apply(func, MakeComponentTuple(entityID, inds)); }
			
			else static_assert(DependentFalse<Func>::value, "Invalid for each function");
		}

	public:

		View(ref<ECS> ecs) : m_ECS(ecs), m_Empty(false)
		{
			// Get a list of lambas that each return the name of a component type
			auto hashLamdas = MakeGetters(std::tuple<Types...>());

			// Get the mask and pools
			for (u64 i = 0; i < hashLamdas.size(); ++i) {
				u64 hash = hashLamdas[i]();
				if (!m_ECS->m_ComponentPools.count(hash)) {
					m_Empty = true;
					return;
				}
				m_ComponentPools[i] = m_ECS->m_ComponentPools[hash];
				m_ComponentMask.set(m_ECS->m_ComponentPoolBits[hash], true);
			}

			// Find the smallest pool
			m_Smallest = *std::min_element(m_ComponentPools.begin(), m_ComponentPools.end(), 
				[](ref<IComponentPool> left, ref<IComponentPool> right) { return left->Size() < right->Size(); }
			);
		}

		void ForEach(std::function<void(Types&...)> func) { ForEachImpl(func); }
		void ForEach(std::function<void(EntityID, Types&...)> func) { ForEachImpl(func); }

		u64 Count() { return m_Smallest->Size(); }
		bool Empty() { return m_Empty; }

	private:
		ref<ECS> m_ECS;
		ECS::ComponentMask m_ComponentMask; // What components the view is looking at
		std::array<ref<IComponentPool>, sizeof...(Types)> m_ComponentPools; // Pointers to the component pools
		ref<IComponentPool> m_Smallest; // Pointer component pool with the fewest components
		bool m_Empty; // Set to true if any of the component pools are empty/dont exist
	};

}
