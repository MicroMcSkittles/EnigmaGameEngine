#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/Components.h>
#include <Enigma/Engine/Physics/PhysicsEngine2D.h>
#include <Enigma/Engine/DeltaTime.h>
#include <Enigma/Engine/UUID.h>

#include "Panels/InspectorPanel.h"

namespace Enigma::Editor {

	class Entity;
	class SceneSerializer;
	struct EntityMetaData;
	struct SceneMetaData;

	class Scene {
	public:
		static ref<Scene> Create();
		Scene();
		Scene(const std::string& name);
		Scene(const Scene& other);
		~Scene();

		// Create/Remove functions
		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntity(const std::string& name, Engine::UUID uuid);
		Entity CreateEntity(Entity parent, const std::string& name = "");
		Entity CreateEntity(Entity parent, const std::string& name, Engine::UUID uuid);
		void RemoveEntity(Entity entity);
		
		// Parent/Child functions
		void ChangeParent(Entity entity, Entity parent);
		bool IsChild(Entity entity, Entity parent);

		void StartRuntime();
		void EndRuntime();
		void Update(Engine::DeltaTime deltaTime);

		void ForEach(std::function<void(Entity)> func);
		void ForEach(std::function<void(Entity, EntityMetaData&)> func);

		// Returns the entity with a uuid
		Entity GetEntity(Engine::UUID uuid);

		// Getters
		EntityMetaData& GetRoot();
		SceneMetaData& GetMetaData();
		std::string& GetName();
		std::string& GetFileName() { return m_FileName; }
		ref<Engine::ECS::ECS> GetECS() { return m_ECS; }

	private:
		ref<Engine::ECS::ECS> m_ECS;
		ref<Engine::Physics::PhysicsEngine2D> m_PhysicsEngine;
		std::unordered_map<Engine::UUID, Entity> m_EntityUUIDs;
		std::string m_FileName;
		bool m_Running;

		friend class Entity;
		friend class SceneSerializer;
	};

	class SceneInspectorContext : public InspectorContext {
	public:
		static ref<SceneInspectorContext> Create(ref<Scene> scene) { return CreateRef<SceneInspectorContext>(scene); }
		SceneInspectorContext(ref<Scene> scene);

		virtual void ShowGui() override;

	private:
		ref<Scene> m_Scene;
	};

}