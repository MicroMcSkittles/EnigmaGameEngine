#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include <Enigma/Engine/ECS/Components.h>
#include <Enigma/Engine/UUID.h>

using namespace Enigma::Engine::ECS;

namespace Enigma::Editor {

	ref<Scene> Scene::Create()
	{
		return CreateRef<Scene>();
	}
	Scene::Scene()
	{
		m_ECS = Engine::ECS::ECS::Create();
		Entity root = { m_ECS->CreateEntity(), this };
		root.CreateComponent<EntityMetaData>("Scene Root");
	}

	EntityMetaData& Scene::GetRoot()
	{
		return Entity(0, this).GetComponent<EntityMetaData>();
	}

	ref<Engine::ECS::ECS> Scene::GetECS()
	{
		return m_ECS;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_ECS->CreateEntity(), this };
		entity.CreateComponent<EntityMetaData>(name);
		entity.CreateComponent<Engine::UUID>();
		entity.CreateComponent<Transform>();

		EntityMetaData& root = Entity(0, this).GetComponent<EntityMetaData>();
		root.children.Create(entity.GetID(), entity);

		return entity;
	}
	Entity Scene::CreateEntity(Entity parent, const std::string& name)
	{
		// Create child entity
		Entity entity = { m_ECS->CreateEntity(), this };
		entity.CreateComponent<EntityMetaData>(name, parent);
		entity.CreateComponent<Engine::UUID>();

		// Create and set transform
		Transform& transform = entity.CreateComponent<Transform>();
		transform.parent = parent.GetID();

		// Update parent entity
		EntityMetaData& metaData = parent.GetComponent<EntityMetaData>();
		metaData.children.Create(entity.GetID(), entity);

		return entity;
	}

	void Scene::ChangeParent(Entity entity, Entity parent)
	{
		EntityMetaData& entityMetaData = entity.GetComponent<EntityMetaData>();

		// Make sure the new parent isn't the same as the old one
		if (entityMetaData.parent == parent) return;

		// Update old parent metadata
		if (entityMetaData.parent) {
			EntityMetaData& originalParent = entityMetaData.parent.GetComponent<EntityMetaData>();
			originalParent.children.Remove(entity.GetID());
		}
		else {
			EntityMetaData& root = Entity(0, this).GetComponent<EntityMetaData>();
			root.children.Remove(entity.GetID());
		}

		// Update new parent metadata
		if (parent) {
			EntityMetaData& parentMetaData = parent.GetComponent<EntityMetaData>();
			parentMetaData.children.Create(entity.GetID(), entity);
		}
		else {
			EntityMetaData& root = Entity(0, this).GetComponent<EntityMetaData>();
			root.children.Create(entity.GetID(), entity);
		}

		// Update entity metadata and transform
		entityMetaData.parent = parent;
		Transform& transform = entity.GetComponent<Transform>();
		transform.parent = parent.GetID();
	}

	bool Scene::IsChild(Entity entity, Entity parent)
	{
		if (!entity || !parent) return false;

		EntityMetaData& metaData = entity.GetComponent<EntityMetaData>();
		if (metaData.parent == parent) return true;

		return IsChild(metaData.parent, parent);
	}

	void Scene::RemoveEntity(Entity entity)
	{
		if (!entity) return;

		// update entity and parent entity metadata
		EntityMetaData& entityMetaData = entity.GetComponent<EntityMetaData>();
		if (entityMetaData.parent) {
			EntityMetaData& parentMetaData = entityMetaData.parent.GetComponent<EntityMetaData>();
			parentMetaData.children.Remove(entity.GetID());
		}
		else {
			EntityMetaData& root = Entity(0, this).GetComponent<EntityMetaData>();
			root.children.Remove(entity.GetID());
		}

		m_ECS->RemoveEntity(entity.GetID());
	}

	void Scene::ForEach(std::function<void(Entity)> func)
	{
		View<Transform> entities(m_ECS);
		entities.ForEach([&](EntityID id, Transform& transform) {
			func({ id, this });
		});
	}
	void Scene::ForEach(std::function<void(Entity, EntityMetaData&)> func)
	{
		View<EntityMetaData> entities(m_ECS);
		entities.ForEach([&](EntityID id, EntityMetaData& metaData) {
			func({ id, this }, metaData);
		});
	}

	void Scene::Update(Engine::DeltaTime deltaTime)
	{

	}
}