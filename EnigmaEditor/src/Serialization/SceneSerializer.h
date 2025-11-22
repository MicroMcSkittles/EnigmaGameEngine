#pragma once
#include "Scene/Scene.h"

namespace YAML {
	class Node;
	class Emitter;
}

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

	private:
		void SerializeEntityMetaData(YAML::Emitter& out, EntityMetaData& entityMetaData);
		void SerializeTransform(YAML::Emitter& out, Engine::ECS::Transform& transform);
		void SerializeColoredQuad(YAML::Emitter& out, Engine::ECS::ColoredQuad& quad);
		void SerializeEntity(YAML::Emitter& out, Entity entity);

		void DeserializeEntityMetaData(const YAML::Node& data, EntityMetaData& metaData);
		void DeserializeTransform(const YAML::Node& data, Engine::ECS::Transform& transform);
		void DeserializeColoredQuad(const YAML::Node& data, Engine::ECS::ColoredQuad& quad);

	private:
		ref<Scene> m_Scene;
	};

}