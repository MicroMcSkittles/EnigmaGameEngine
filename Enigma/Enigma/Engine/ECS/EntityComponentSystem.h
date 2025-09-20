#pragma once
#include "Enigma/Core/Core.h"
#include "Enigma/Core/SparseSet.h"
#include <tuple>
#include <algorithm>
#include <bitset>

// Times writen: 3
// I'm going insaine
// I am like a room full of monkies. I am destined to type for a infinite amount
// of time to get any kind of result with this system

// Times writen: 4
// Why must I subject myself to this
// 
// It's so much better now!
//
// On my pc it can go up to 500000 entities at 60 fps on release mode

namespace Enigma::Engine::ECS {

	typedef   size_t   EntityID;
	constexpr EntityID InvalidEntityID = -1;
	constexpr size_t   ComponentPoolPageSize = 64;
	constexpr size_t   MaxComponentPoolCount = 64;

	class IComponentPool {
	public:
		virtual void Create(EntityID entityID) = 0;
		virtual void Remove(EntityID entityID) = 0;
		virtual bool Has(EntityID entityID) = 0; // returns true if the entity has a component in the component pool
		virtual size_t Size() = 0;
		virtual std::vector<size_t>& GetIDs() = 0;
	};

	template<typename T>
	class ComponentPool : public IComponentPool {
	public:
		
		virtual void Create(EntityID entityID) override { m_Components.Create(entityID); }
		virtual void Remove(EntityID entityID) override { m_Components.Remove(entityID); }
		virtual bool Has(EntityID entityID) override { return m_Components.Contains(entityID); }
		virtual size_t Size() override { return m_Components.Size(); }
		virtual std::vector<size_t>& GetIDs() override { return m_Components.GetIDs(); }

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
		using ComponentMask = std::bitset<MaxComponentPoolCount>;

	public:

		ECS() {
			m_EntityCount = 0;
		}
		~ECS() {
			for (auto& [type, pool] : m_ComponentPools) {
				delete pool;
			}
		}

		size_t GetEntityCount() {
			return m_EntityCount;
		}

		template<typename... Types> 
		View<Types...> GetView() {
			return View<Types...>(this);
		}
		template<typename T>
		ComponentPool<T>* GetComponentPool() {
			// Make sure component pool exists
			std::string componentType = typeid(T).name();
			LOG_ASSERT(!m_ComponentPools.count(componentType), "Failed to get component pool");

			// Get the pool
			return (ComponentPool<T>*)m_ComponentPools[componentType];
		}
		template<typename T>
		T& GetComponent(EntityID entityID) {
			ComponentPool<T>* pool = GetComponentPool<T>();
			return pool->GetComponent(entityID);
		}

		template<typename T>
		T& CreateComponent(EntityID entityID) {
			std::string componentType = typeid(T).name();

			// If component pool for component type doesn't exist than create one
			if (!m_ComponentPools.count(componentType)) {
				m_ComponentPoolBits.insert({ componentType, m_ComponentPools.size() });
				m_ComponentPools.insert({ componentType, new ComponentPool<T>() });
			}

			// Update bit mask and entity groups
			ComponentMask& mask = m_EntityComponentMasks.Get(entityID);
			if (m_EntityGroups.count(mask) && m_EntityGroups[mask].Contains(entityID)) {
				m_EntityGroups[mask].Remove(entityID);
			}
			mask[m_ComponentPoolBits[componentType]] = true;
			if (!m_EntityGroups.count(mask)) m_EntityGroups.insert({ mask, {} });
			m_EntityGroups[mask].Create(entityID, entityID);

			// Get the component pool
			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentType];
			pool->Create(entityID);
			return pool->GetComponent(entityID);
		}
		template<typename T>
		void RemoveComponent(EntityID entityID) {
			std::string componentType = typeid(T).name();

			LOG_ASSERT(!m_ComponentPools.count(componentType), "Failed to remove component");

			// Update bit mask and entity group
			ComponentMask& mask = m_EntityComponentMasks.Get(entityID);
			m_EntityGroups[mask].Remove(entityID);
			mask[m_ComponentPoolBits[componentType]] = false;
			if (!m_EntityGroups.count(mask)) m_EntityGroups.insert({ mask, {} });
			m_EntityGroups[mask].Create(entityID, entityID);

			// Get the component pool
			ComponentPool<T>* pool = (ComponentPool<T>*)m_ComponentPools[componentType];
			pool->Remove(entityID);
		}

		template<typename T>
		bool HasComponent(EntityID entityID) {
			std::string componentType = typeid(T).name();
			if (!m_ComponentPools.count(componentType)) return false;
			if (!m_EntityComponentMasks.Contains(entityID)) return false;
			if (!m_EntityComponentMasks.Get(entityID)[m_ComponentPoolBits[componentType]]) return false;
			return true;
		}

		EntityID CreateEntity() {
			EntityID availableID = InvalidEntityID;

			// If there is a available entity id than take it and remove it from the m_AvailableEntityIDs
			if (!m_AvailableEntityIDs.empty()) {
				for (size_t i = 0; i < m_AvailableEntityIDs.size(); ++i) {
					if (m_AvailableEntityIDs[i] < m_EntityCount + 1) {
						availableID = m_AvailableEntityIDs[i];
						m_AvailableEntityIDs.erase(m_AvailableEntityIDs.begin() + i);
						break;
					}
				}
			}
			// If there where no available entity ids then create a new one
			else {
				availableID = m_EntityCount;
			}
			m_EntityCount += 1;

			m_EntityComponentMasks.Create(availableID);

			return availableID;
		}
		void RemoveEntity(EntityID entityID) {
			// Remove all the components that made up entity id
			for (auto& [type, pool] : m_ComponentPools) {
				if (!pool->Has(entityID)) continue;
				pool->Remove(entityID);
			}

			ComponentMask& mask = m_EntityComponentMasks.Get(entityID);
			m_EntityGroups[mask].Remove(entityID);
			m_EntityComponentMasks.Remove(entityID);

			m_AvailableEntityIDs.push_back(entityID);
			m_EntityCount -= 1;
		}

	private:
		size_t m_EntityCount;
		std::vector<EntityID> m_AvailableEntityIDs;

		std::unordered_map<std::string, IComponentPool*> m_ComponentPools;
		std::unordered_map<std::string, size_t> m_ComponentPoolBits;

		Core::SparseSet<ComponentMask> m_EntityComponentMasks;
		std::unordered_map<ComponentMask, Core::SparseSet<EntityID>> m_EntityGroups;
	};

	template<typename... Types>
	class View {
	private:
		// ??? huh, this is black magic
		template <typename Tuple, size_t... Indices>
		auto MakeGettersImpl(Tuple& t, std::index_sequence<Indices...>) {
			return std::vector<std::function<std::string()>> {
				[&t]() { return std::string(typeid(std::get<Indices>(t)).name()); }...
			};
		}
		template <typename Tuple>
		auto MakeGetters(Tuple t) {
			return MakeGettersImpl(t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
		}

		template<size_t Index>
		auto GetPoolAt() {
			return static_cast<ComponentPool<std::tuple_element_t<Index, std::tuple<Types...>>>*>(m_ComponentPools[Index]);
		}
		template<size_t... Indices>
		auto MakeComponentTuple(EntityID id, std::index_sequence<Indices...>) {
			return std::make_tuple((std::ref(GetPoolAt<Indices>()->GetComponent(id)))...);
		}
		template<typename Func>
		void ForEachImpl(Func func) {
			constexpr auto inds = std::make_index_sequence<sizeof...(Types)>{};
			for (EntityID& entityID : m_Smallest->GetIDs()) {
				if constexpr (std::is_invocable_v<Func, EntityID, Types&...>) {
					std::apply(func, std::tuple_cat(std::make_tuple(entityID), MakeComponentTuple(entityID, inds)));
				}
				else if constexpr (std::is_invocable_v<Func, Types&...>) {
					std::apply(func, MakeComponentTuple(entityID, inds));
				}
				else LOG_ERROR("Invalid for each function");
			}
		}

	public:

		View(ECS* ecs)
		{
			auto getters = MakeGetters(std::tuple<Types...>());
			m_ECS = ecs;

			// Get the mask and pools
			for (size_t i = 0; i < getters.size(); ++i) {
				std::string type = getters[i]();
				m_ComponentPools[i] = m_ECS->m_ComponentPools[type];
				m_ComponentMask.set(m_ECS->m_ComponentPoolBits[type], true);
			}

			// Find the smallest pool
			m_Smallest = *std::min_element(m_ComponentPools.begin(), m_ComponentPools.end(), 
				[](IComponentPool* left, IComponentPool* right) { return left->Size() < right->Size(); }
			);
		}

		void ForEach(std::function<void(Types&...)> func) {
			ForEachImpl(func);
		}
		void ForEach(std::function<void(EntityID, Types&...)> func) {
			ForEachImpl(func);
		}

	private:
		ECS::ComponentMask m_ComponentMask;
		ECS* m_ECS;
		std::array<IComponentPool*, sizeof...(Types)> m_ComponentPools;
		IComponentPool* m_Smallest;
	};

}
