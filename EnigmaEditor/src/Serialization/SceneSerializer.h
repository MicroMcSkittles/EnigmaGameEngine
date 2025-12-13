#pragma once
#include "Scene/Scene.h"
#include <Enigma/Engine/Physics/PhysicsComponents.h>

#include <yaml-cpp/yaml.h>

namespace Enigma::Editor {

	class SceneSerializer {
	public:
		// The extension used for scene files
		static std::string FileExtension;

	public:
		SceneSerializer(const ref<Scene>& scene);

		void Serialize(const std::string& filename);
		void SerializeBinary(const std::string& filename);

		bool Deserialize(const std::string& filename);
		bool DeserializeBinary(const std::string& filename);

		YAML::Node SerializeEntityToNode(Entity entity);
		Entity DeserializeEntityFromNode(const YAML::Node& data);

	private:
		void SerializeEntityMetaData(YAML::Emitter& out, Entity entity, EntityMetaData& entityMetaData);
		void SerializeTransform(YAML::Emitter& out, Entity entity, Engine::ECS::TransformComponent& transform);
		void SerializeOrthographicCamera(YAML::Emitter& out, Entity entity, Engine::ECS::OrthographicCameraComponent& camera);
		void SerializeQuadRenderer(YAML::Emitter& out, Entity entity, Engine::ECS::QuadRendererComponent& quad);
		void SerializeCircleRenderer(YAML::Emitter& out, Entity entity, Engine::ECS::CircleRendererComponent& circle);
		void SerializeRidgidBody2D(YAML::Emitter& out, Entity entity, Engine::Physics::RidgidBody2D& ridgidBody);
		void SerializeEntity(YAML::Emitter& out, Entity entity);

		void DeserializeEntityMetaData(const YAML::Node& data, Entity entity, EntityMetaData& metaData);
		void DeserializeTransform(const YAML::Node& data, Entity entity, Engine::ECS::TransformComponent& transform);
		void DeserializeOrthographicCamera(const YAML::Node& data, Entity entity, Engine::ECS::OrthographicCameraComponent& camera);
		void DeserializeQuadRenderer(const YAML::Node& data, Entity entity, Engine::ECS::QuadRendererComponent& quad);
		void DeserializeCircleRenderer(const YAML::Node& data, Entity entity, Engine::ECS::CircleRendererComponent& circle);
		void DeserializeRidgidBody2D(const YAML::Node& data, Entity entity, Engine::Physics::RidgidBody2D& ridgidBody);
		void DeserializeEntity(const YAML::Node& data, Entity entity);

	private:
		ref<Scene> m_Scene;
	};

}