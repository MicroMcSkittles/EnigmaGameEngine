#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/Components.h>
#include <Enigma/Engine/DeltaTime.h>
#include <Enigma/Engine/UUID.h>

namespace Enigma::Editor {

	class Entity;
	class SceneSerializer;
	struct EntityMetaData;

	class Scene {
	public:
		static ref<Scene> Create();
		Scene();
		Scene(const std::string& name);
		~Scene();

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntity(Entity parent, const std::string& name = "");
		void RemoveEntity(Entity entity);
		void ChangeParent(Entity entity, Entity parent);

		bool IsChild(Entity entity, Entity parent);

		void ForEach(std::function<void(Entity)> func);
		void ForEach(std::function<void(Entity, EntityMetaData&)> func);

		void Update(Engine::DeltaTime deltaTime);

		// Returns the entity with a uuid
		Entity GetEntity(Engine::UUID uuid);

		EntityMetaData& GetRoot();
		ref<Engine::ECS::ECS> GetECS();

		std::string& GetName();
		std::string& GetFileName();

	private:
		ref<Engine::ECS::ECS> m_ECS;
		std::unordered_map<Engine::UUID, Entity> m_EntityUUIDs;
		std::string m_FileName;

		friend class Entity;
		friend class SceneSerializer;
	};

}