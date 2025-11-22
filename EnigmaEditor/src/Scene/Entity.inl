#include "Entity.h"


namespace Enigma::Editor {
	
	template<typename T>
	inline T& Entity::GetComponent() {
		return m_Scene->m_ECS->GetComponent<T>(m_EntityID);
	}

	template<typename T, typename ...Args>
	inline T& Entity::CreateComponent(Args && ...args) {
		return m_Scene->m_ECS->CreateComponent<T>(m_EntityID, std::forward<Args>(args)...);
	}

	template<typename T>
	inline void Entity::RemoveComponent() {
		m_Scene->m_ECS->RemoveComponent<T>(m_EntityID);
	}

	template<typename T>
	inline bool Entity::HasComponent() const {
		return m_Scene->m_ECS->HasComponent<T>(m_EntityID);
	}
}