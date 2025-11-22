#include "Serialization/SceneSerializer.h"
#include "Scene/Components.h"

#include <Enigma/Core/System.h>
#include <yaml-cpp/yaml.h>
#include <fstream>


namespace YAML {
	// Custom YAML converters
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::quat> {
		static Node encode(const glm::quat& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::quat& rhs) {
			if (!node.IsSequence() || node.size() != 4) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
	template<>
	struct convert<Enigma::Engine::UUID> {
		static Node encode(const Enigma::Engine::UUID& rhs) {
			Node node;
			node.push_back(static_cast<u64>(rhs));
			return node;
		}

		static bool decode(const Node& node, Enigma::Engine::UUID& rhs) {
			if (!node.IsScalar()) return false;

			rhs = Enigma::Engine::UUID(node.as<u64>());
			return true;
		}
	};

	// Custom Emitter operators
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const Enigma::Engine::UUID& uuid) {
		out << static_cast<u64>(uuid);
		return out;
	}
}

namespace Enigma::Editor {

	using namespace Enigma::Engine;

	SceneSerializer::SceneSerializer(const ref<Scene>& scene) :m_Scene(scene) { }

	// Serialization
	template<typename T>
	static void SerializeComponent(YAML::Emitter& out, const ref<Scene>& scene, Entity entity, std::function<void(YAML::Emitter&, const ref<Scene>&, T&)> func) {
		if (!entity.HasComponent<T>()) return;

		T& component = entity.GetComponent<T>();
		func(out, scene, component);
	}
	
	static void SerializeEntityMetaData(YAML::Emitter& out, const ref<Scene>& scene, EntityMetaData& entityMetaData) {
		out << YAML::Key << "EntityMetaData" << YAML::Comment("Extra data needed by the editor");
		out << YAML::BeginMap;
		
		out << YAML::Key << "Name" << YAML::Value << entityMetaData.name;
		if (entityMetaData.parent) {
			out << YAML::Key << "Parent" << YAML::Value << entityMetaData.parent.GetUUID();
		}

		if (!entityMetaData.children.Empty()) {
			out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
			for (Entity& child : entityMetaData.children.GetData()) out << child.GetUUID();
			out << YAML::EndSeq;
		}

		out << YAML::Key << "Degrees" << YAML::Value << entityMetaData.degrees;
		out << YAML::Key << "EulerAngles" << entityMetaData.eulerAngles;

		out << YAML::EndMap;
	}
	static void SerializeTransform(YAML::Emitter& out, const ref<Scene>& scene, ECS::Transform& transform) {
		out << YAML::Key << "Transform";
		out << YAML::BeginMap;
		
		out << YAML::Key << "Position" << YAML::Value << transform.position;
		out << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
		out << YAML::Key << "Scale"    << YAML::Value << transform.scale;

		Entity parent = { transform.parent, scene.get() };
		if (parent) out << YAML::Key << "Parent" << YAML::Value << parent.GetUUID();

		out << YAML::EndMap;
	}
	static void SerializeColoredQuad(YAML::Emitter& out, const ref<Scene>& scene, ECS::ColoredQuad& quad) {
		out << YAML::Key << "ColoredQuad";
		out << YAML::BeginMap;

		out << YAML::Key << "Tint" << YAML::Value << quad.tint;

		out << YAML::EndMap;
	}

	static void SerializeEntity(YAML::Emitter& out, const ref<Scene>& scene, Entity entity) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		SerializeComponent<EntityMetaData>(out, scene, entity, SerializeEntityMetaData);
		SerializeComponent<ECS::Transform>(out, scene, entity, SerializeTransform);
		SerializeComponent<ECS::ColoredQuad>(out, scene, entity, SerializeColoredQuad);

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filename)
	{
		YAML::Emitter out;
		out << YAML::Comment("An Enigma scene file");

		out << YAML::BeginMap;
		// Scene name
		out << YAML::Key << "Scene" << YAML::Value << "Untitled, little do they know you can't title scenes yet";

		// Entities
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->ForEach([&](Entity entity) {
			SerializeEntity(out, m_Scene, entity);
		});
		out << YAML::EndSeq;

		out << YAML::EndMap;

		// Open file
		std::ofstream file;
		file.open(filename);
		if (!file.is_open()) {
			LOG_SOFT_ERROR("Failed to save to file ( %s )");
		}

		// Save and close file
		file << out.c_str();
		file.close();
	}
	void SceneSerializer::SerializeBinary(const std::string& filename)
	{
		LOG_WARNING("function ( SceneSerializer::SerializeBinary(const std::string& filename) ) not impl yet!");
	}

	// Deserialization
	template<typename T>
	static void DeserializeComponent(const std::string& name, Entity entity, std::unordered_map<Engine::UUID, Entity>& entities, const YAML::Node& data, std::function<void(const YAML::Node&, std::unordered_map<Engine::UUID, Entity>&, T&)> func) {
		YAML::Node componentData = data[name];
		if (!componentData) return;

		T& component = entity.CreateComponent<T>();
		func(componentData, entities, component);
	}

	static void DeserializeEntityMetaData(const YAML::Node& data, std::unordered_map<Engine::UUID, Entity>& entities, EntityMetaData& metaData) {
		metaData.name = data["Name"].as<std::string>();

		if (data["Parent"]) {
			Engine::UUID parentID = data["Parent"].as<Engine::UUID>();
			metaData.parent = entities[parentID];
		}
		
		if (data["Children"]) {
			u64 childCount = 0;
			for (const YAML::Node& child : data["Children"]) {
				Engine::UUID childID = child.as<Engine::UUID>();
				metaData.children.Create(childCount++, entities[childID]);
			}
		}

		metaData.degrees = data["Degrees"].as<bool>();
		metaData.eulerAngles = data["EulerAngles"].as<glm::vec3>();
	}
	static void DeserializeTransform(const YAML::Node& data, std::unordered_map<Engine::UUID, Entity>& entities, ECS::Transform& transform) {
		transform.position = data["Position"].as<glm::vec3>();
		transform.rotation = data["Rotation"].as<glm::quat>();
		transform.scale    = data["Scale"].as<glm::vec3>();
		if (data["Parent"]) {
			Engine::UUID uuid = data["Parent"].as<Engine::UUID>();
			transform.parent = entities[uuid].GetID();
		}
	}
	static void DeserializeColoredQuad(const YAML::Node& data, std::unordered_map<Engine::UUID, Entity>& entities, ECS::ColoredQuad& quad) {
		quad.tint = data["Tint"].as<glm::vec3>();
	}

	bool SceneSerializer::Deserialize(const std::string& filename)
	{
		// Read YAML file
		std::string fileContent = Core::System::ReadFileStr(filename);
		YAML::Node data = YAML::Load(fileContent);

		// Make sure it is a Scene file
		if (!data["Scene"]) {
			LOG_WARNING("Failed to deserialize scene ( %s ) invalid format", filename.c_str());
			return false;
		}

		// Get Scene name
		std::string sceneName = data["Scene"].as<std::string>();
		LOG_MESSAGE("Deserializing scene \"%s\" from ( %s )", 5, sceneName.c_str(), filename.c_str());
		
		YAML::Node entities = data["Entities"];
		std::unordered_map<Engine::UUID, Entity> entityUUIDMap;
		if (entities) {
			// Create an entity for every entity in the file
			for (YAML::Node entityData : entities) {
				// Create entity
				Entity entity = { m_Scene->m_ECS->CreateEntity(), m_Scene.get() };
				if (!entityData["EntityMetaData"]["Parent"]) {
					EntityMetaData& sceneRoot = Entity(0, m_Scene.get()).GetComponent<EntityMetaData>();
					sceneRoot.children.Create(entity.GetID(), entity);
				}

				// Create UUID
				Engine::UUID uuid = entityData["Entity"].as<Engine::UUID>();
				entity.CreateComponent<Engine::UUID>(uuid);

				entityUUIDMap.insert({ uuid, entity });
			}

			// Load the components for each entity
			for (YAML::Node entityData : entities) {

				Engine::UUID uuid = entityData["Entity"].as<Engine::UUID>();
				Entity entity = entityUUIDMap[uuid];

				// Deserialize components
				DeserializeComponent<EntityMetaData>("EntityMetaData", entity, entityUUIDMap, entityData, DeserializeEntityMetaData);
				DeserializeComponent<ECS::Transform>("Transform", entity, entityUUIDMap, entityData, DeserializeTransform);
				DeserializeComponent<ECS::ColoredQuad>("ColoredQuad", entity, entityUUIDMap, entityData, DeserializeColoredQuad);
			
				LOG_MESSAGE("Deserialized entity \"%s\", UUID: %u", 5, entity.GetComponent<EntityMetaData>().name.c_str(), uuid);
			}
		}

		return true;
	}
	bool SceneSerializer::DeserializeBinary(const std::string& filename)
	{
		LOG_WARNING("function ( SceneSerializer::DeserializeBinary(const std::string& filename) ) not impl yet!");
		return false;
	}

}