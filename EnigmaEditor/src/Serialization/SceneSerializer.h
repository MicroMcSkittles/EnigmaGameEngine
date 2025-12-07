#pragma once
#include "Scene/Scene.h"

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
		void SerializeTransform(YAML::Emitter& out, Entity entity, Engine::ECS::Transform& transform);
		void SerializeOrthographicCamera(YAML::Emitter& out, Entity entity, Engine::ECS::OrthographicCamera& camera);
		void SerializeColoredQuad(YAML::Emitter& out, Entity entity, Engine::ECS::ColoredQuad& quad);
		void SerializeEntity(YAML::Emitter& out, Entity entity);

		void DeserializeEntityMetaData(const YAML::Node& data, Entity entity, EntityMetaData& metaData);
		void DeserializeTransform(const YAML::Node& data, Entity entity, Engine::ECS::Transform& transform);
		void DeserializeOrthographicCamera(const YAML::Node& data, Entity entity, Engine::ECS::OrthographicCamera& camera);
		void DeserializeColoredQuad(const YAML::Node& data, Entity entity, Engine::ECS::ColoredQuad& quad);
		void DeserializeEntity(const YAML::Node& data, Entity entity);

	private:
		ref<Scene> m_Scene;
	};

}