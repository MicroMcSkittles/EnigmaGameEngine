#pragma once
#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/Components.h>
#include <Enigma/Engine/DeltaTime.h>

namespace Enigma::Editor {

	class Entity;
	class SceneSerializer;
	struct EntityMetaData;

	class Scene {
	public:
		static ref<Scene> Create();
		Scene();

		EntityMetaData& GetRoot();
		ref<Engine::ECS::ECS> GetECS();

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntity(Entity parent, const std::string& name = "");
		void RemoveEntity(Entity entity);
		void ChangeParent(Entity entity, Entity parent);

		bool IsChild(Entity entity, Entity parent);

		void ForEach(std::function<void(Entity)> func);
		void ForEach(std::function<void(Entity, EntityMetaData&)> func);

		void Update(Engine::DeltaTime deltaTime);

	private:
		ref<Engine::ECS::ECS> m_ECS;

		friend class Entity;
		friend class SceneSerializer;
	};

}