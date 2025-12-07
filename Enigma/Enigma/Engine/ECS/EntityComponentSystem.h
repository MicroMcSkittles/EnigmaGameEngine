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

	class ComponentPoolInterface {
	public:
		virtual ~ComponentPoolInterface() { }

		virtual void Create(EntityID entityID) = 0;
		virtual void Remove(EntityID entityID) = 0;
		virtual bool Contains(EntityID entityID) = 0; // returns true if the entity has a component in the component pool
		virtual u64 Size() = 0;
		virtual std::vector<u64>& GetIDs() = 0;
	};

	template<typename T>
	class ComponentPool : public ComponentPoolInterface {
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

	// Component hashing helper classes
	template<typename T>
	class ComponentHasher {
	public:
		static u64 Hash() { return reinterpret_cast<u64>(&s_ID); }
	private:
		inline static char s_ID;
	};
	template<typename... Types>
	class ComponentHashGetter {
	private:
		template <typename Tuple, u64... Indices> static auto MakeGettersImpl(Tuple& t, std::index_sequence<Indices...>);

	public:
		// Returns a list of lamdas that return the hashes of the component pools
		template <typename Tuple> static auto MakeGetters(Tuple t);
	};

	template<typename...> class View;

	class ECS {
	public:
		static ref<ECS> Create();
		ECS();
		ECS(const ECS& other);
		~ECS();

		template<typename T> ref<ComponentPool<T>> GetComponentPool();
		template<typename T> T& GetComponent(EntityID entityID);

		template<typename T, typename... Args> T& CreateComponent(EntityID entityID, Args... constructorArgs);
		template<typename T> void RemoveComponent(EntityID entityID);
		template<typename T> bool HasComponent(EntityID entityID);
		template<typename... Comps> bool HasComponents(EntityID entityID);

		EntityID CreateEntity();
		void RemoveEntity(EntityID entityID);

		u64 GetEntityCount();

	private:
		using ComponentMask = std::bitset<MaxComponentPoolCount>;
		template<typename...> friend class View;

	private:
		std::unordered_map<u64, ref<ComponentPoolInterface>> m_ComponentPools; // Maps component pool hashes to component pools
		std::unordered_map<u64, u64> m_ComponentPoolMaskBits; // Maps component pool hashes to a bit in a component mask
		
		Core::SparseSet<ComponentMask> m_EntityComponentMasks;
		std::unordered_map<ComponentMask, Core::SparseSet<EntityID>> m_EntityGroups; // Maps a component mask to a list of entities that have that mask
		
		std::vector<EntityID> m_AvailableEntityIDs; // All unused entity ids
		u64 m_EntityCount;
	};

	template<typename... Types>
	class View {
	private:
		// Used for calling static asserts, this makes the static asserts evaluate at compile time
		template<class T>
		struct DependentFalse : std::false_type { };


		template<u64 Index> auto GetPoolAt();
		template<u64... Indices> auto MakeComponentTuple(EntityID id, std::index_sequence<Indices...>);

		// Runs a function lamda for each entity being viewed
		template<typename Func> void ForEachImpl(Func func);

	public:
		View(ref<ECS> ecs);
		~View() { }

		void ForEach(std::function<void(Types&...)> func) { ForEachImpl(func); }
		void ForEach(std::function<void(EntityID)> func) { ForEachImpl(func); }
		void ForEach(std::function<void(EntityID, Types&...)> func) { ForEachImpl(func); }

		u64 Count() { return m_Smallest->Size(); }
		bool Empty() { return m_Empty; }

	private:
		ref<ECS> m_ECS;
		ECS::ComponentMask m_ComponentMask; // What components the view is looking at
		std::array<ref<ComponentPoolInterface>, sizeof...(Types)> m_ComponentPools; // Pointers to the component pools
		ref<ComponentPoolInterface> m_Smallest; // Pointer component pool with the fewest components
		bool m_Empty; // Set to true if any of the component pools are empty/dont exist
	};

}

#include "Enigma/Engine/ECS/EntityComponentSystem.inl"
