
namespace Enigma::Engine::ECS {

	// Entity Component System Class ======================
	template<typename T> inline ref<ComponentPool<T>> ECS::GetComponentPool() {
		// Make sure component pool exists
		u64 componentHash = ComponentHasher<T>::Hash();
		LOG_ASSERT(!m_ComponentPools.count(componentHash), "Failed to get component pool");

		// Get the pool
		return CastRef<ComponentPool<T>>(m_ComponentPools[componentHash]);
	}
	template<typename T> inline T& ECS::GetComponent(EntityID entityID) {
		ref<ComponentPool<T>> pool = GetComponentPool<T>();
		return pool->GetComponent(entityID);
	}
	
	template<typename T, typename... Args> inline T& ECS::CreateComponent(EntityID entityID, Args... constructorArgs) {
		u64 componentHash = ComponentHasher<T>::Hash();

		// If component pool for component hash doesn't exist than create one
		if (!m_ComponentPools.count(componentHash)) {
			m_ComponentPoolMaskBits.insert({ componentHash, m_ComponentPools.size() });
			m_ComponentPools.insert({ componentHash, CreateRef<ComponentPool<T>>() });
		}

		// Update bit mask and entity groups
		ComponentMask& mask = m_EntityComponentMasks.Get(entityID);
		if (m_EntityGroups.count(mask) && m_EntityGroups[mask].Contains(entityID)) {
			m_EntityGroups[mask].Remove(entityID);
		}
		mask[m_ComponentPoolMaskBits[componentHash]] = true;
		if (!m_EntityGroups.count(mask)) m_EntityGroups.insert({ mask, { } });
		m_EntityGroups[mask].Create(entityID, entityID);

		// Get the component pool
		ref<ComponentPool<T>> pool = CastRef<ComponentPool<T>>(m_ComponentPools[componentHash]);

		// Create component, use constructor args if provided
		pool->Create(entityID);
		if constexpr (sizeof...(Args) != 0) {
			pool->GetComponent(entityID) = T{ constructorArgs... };
		}
		return pool->GetComponent(entityID);
	}
	template<typename T> inline void ECS::RemoveComponent(EntityID entityID) {
		u64 componentHash = ComponentHasher<T>::Hash();
		LOG_ASSERT(!m_ComponentPools.count(componentHash), "Failed to remove component");

		// Get the entity's component mask
		ComponentMask& mask = m_EntityComponentMasks.Get(entityID);

		// Remove entity from old group
		m_EntityGroups[mask].Remove(entityID);
		if (!m_EntityGroups.count(mask)) m_EntityGroups.insert({ mask, {} });

		// Update bit mask
		mask[m_ComponentPoolMaskBits[componentHash]] = false;

		// Place entity in new group
		m_EntityGroups[mask].Create(entityID, entityID);

		// Get the component pool
		ref<ComponentPool<T>> pool = CastRef<ComponentPool<T>>(m_ComponentPools[componentHash]);
		pool->Remove(entityID);
	}
	template<typename T> inline bool ECS::HasComponent(EntityID entityID) {
		u64 componentHash = ComponentHasher<T>::Hash();
		// Make sure the entity exitsts
		if (!m_EntityComponentMasks.Contains(entityID)) return false;
		// Check if component pool exists
		if (!m_ComponentPools.count(componentHash)) return false;
		// Make sure the entity has that component
		if (!m_EntityComponentMasks.Get(entityID)[m_ComponentPoolMaskBits[componentHash]]) return false;
		return true;
	}

	// Component View Class ===============================
	template<typename... Types> template <typename Tuple, u64... Indices>
	inline auto View<Types...>::MakeComponentHashGettersImpl(Tuple& t, std::index_sequence<Indices...>) {
		return std::vector<std::function<u64()>> {
			[&t]() { return ComponentHasher<std::tuple_element_t<Indices, std::tuple<Types...>>>::Hash(); }...
		};
	}
	template<typename... Types> template <typename Tuple> auto View<Types...>::MakeComponentHashGetters(Tuple t) {
		return MakeComponentHashGettersImpl(t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
	}

	template<typename... Types> template<u64 Index> auto View<Types...>::GetPoolAt() {
		return std::static_pointer_cast<ComponentPool<std::tuple_element_t<Index, std::tuple<Types...>>>>(m_ComponentPools[Index]);
	}
	template<typename... Types> template<u64... Indices> auto View<Types...>::MakeComponentTuple(EntityID id, std::index_sequence<Indices...>) {
		return std::make_tuple((std::ref(GetPoolAt<Indices>()->GetComponent(id)))...);
	}

	template<typename... Types> template<typename Func> void View<Types...>::ForEachImpl(Func func) {
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

	template<typename... Types> View<Types...>::View(ref<ECS> ecs) : m_ECS(ecs), m_Empty(false)
	{
		// Get hash lamdas
		auto hashLamdas = MakeComponentHashGetters(std::tuple<Types...>());

		// Get the mask and pools
		for (u64 i = 0; i < hashLamdas.size(); ++i) {
			u64 hash = hashLamdas[i]();
			if (!m_ECS->m_ComponentPools.count(hash)) {
				m_Empty = true;
				return;
			}
			m_ComponentPools[i] = m_ECS->m_ComponentPools[hash];
			m_ComponentMask.set(m_ECS->m_ComponentPoolMaskBits[hash], true);
		}

		// Find the smallest pool
		m_Smallest = *std::min_element(m_ComponentPools.begin(), m_ComponentPools.end(),
			[](ref<ComponentPoolInterface> left, ref<ComponentPoolInterface> right) { return left->Size() < right->Size(); }
		);
	}
}