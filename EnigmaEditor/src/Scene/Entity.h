#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/UUID.h>

#include "Scene/Scene.h"
#include "Panels/InspectorPanel.h"

namespace Enigma::Editor {

	struct EntityMetaData;

	// Wrapper for the ECS entity
	class Entity {
	public:
		Entity();
		Entity(Engine::ECS::EntityID entityID, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T> T& GetComponent();
		template<typename T> bool HasComponent() const;
		template<typename T> void RemoveComponent();
		template<typename T, typename... Args> T& CreateComponent(Args&&... args);

		bool operator ==(const Entity& other);
		bool operator !=(const Entity& other);

		Engine::ECS::EntityID GetID() const { return m_EntityID; }
		Engine::UUID GetUUID() { return GetComponent<Engine::UUID>(); }

		EntityMetaData& GetMetaData();

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
		void HeaderGui();
		void RenameGui();

	private:
		Entity m_Entity;

		bool m_StartRename;
		bool m_EndRename;
		std::string m_OriginalName;
	};
}

#include "Scene/Entity.inl"