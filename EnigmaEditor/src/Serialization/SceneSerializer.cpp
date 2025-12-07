#include "Serialization/SceneSerializer.h"
#include "Scene/Components.h"

#include <Enigma/Core/System.h>
#include <fstream>
#include <filesystem>

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

	std::string SceneSerializer::FileExtension = ".scene";

	SceneSerializer::SceneSerializer(const ref<Scene>& scene) :m_Scene(scene) { }

	// Serialization
	template<typename T, typename Func>
	static void SerializeComponent(YAML::Emitter& out, Entity entity, Func func, SceneSerializer* serializer) {
		// Check if entity has a component T
		if (!entity.HasComponent<T>()) return;

		// Make sure the function is valid
		if constexpr (!std::is_invocable_v<Func, SceneSerializer&, YAML::Emitter&, Entity, T&>) {
			LOG_ERROR("Invalid serialization function");
		}

		// Call serialization function
		T& component = entity.GetComponent<T>();
		(serializer->*func)(out, entity, component);
	}
	
	void SceneSerializer::SerializeEntityMetaData(YAML::Emitter& out, Entity entity, EntityMetaData& entityMetaData) {
		// Start EntityMetaData Node
		out << YAML::Key << "EntityMetaData" << YAML::Comment("Extra data needed by the editor");
		out << YAML::BeginMap;
		
		// Push name
		out << YAML::Key << "Name" << YAML::Value << entityMetaData.name;

		// Push parent if it exists
		if (entityMetaData.parent) {
			out << YAML::Key << "Parent" << YAML::Value << entityMetaData.parent.GetUUID();
		}

		// Push children if they exist
		if (!entityMetaData.children.Empty()) {
			out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
			for (Entity& child : entityMetaData.children.GetData()) out << child.GetUUID();
			out << YAML::EndSeq;
		}

		out << YAML::EndMap;
	}
	void SceneSerializer::SerializeTransform(YAML::Emitter& out, Entity entity, ECS::Transform& transform) {
		// Start Transform Node
		out << YAML::Key << "Transform";
		out << YAML::BeginMap;

		// Push parent if it exists
		Entity parent = { transform.parent, m_Scene.get() };
		if (parent) out << YAML::Key << "Parent" << YAML::Value << parent.GetUUID();

		// Push transform data
		out << YAML::Key << "Position" << YAML::Value << transform.position;
		out << YAML::Key << "Rotation" << YAML::Value << transform.rotation << YAML::Comment("Quaternion");
		out << YAML::Key << "Scale"    << YAML::Value << transform.scale;

		// Push meta data
		TransformMetaData& metaData = entity.GetComponent<TransformMetaData>();
		out << YAML::Key << "Degrees"     << YAML::Value << metaData.degrees;
		out << YAML::Key << "EulerAngles" << YAML::Value << metaData.eulerAngles << YAML::Comment("Radians");

		out << YAML::EndMap;
	}
	void SceneSerializer::SerializeOrthographicCamera(YAML::Emitter& out, Entity entity, ECS::OrthographicCamera& camera)
	{
		// Start ColoredQuad Node
		out << YAML::Key << "OrthographicCamera";
		out << YAML::BeginMap;

		out << YAML::Key << "Left" << YAML::Value << camera.view.left;
		out << YAML::Key << "Right" << YAML::Value << camera.view.right;
		out << YAML::Key << "Top" << YAML::Value << camera.view.top;
		out << YAML::Key << "Bottom" << YAML::Value << camera.view.bottom;
		
		out << YAML::Key << "Near" << YAML::Value << camera.view.near;
		out << YAML::Key << "Far" << YAML::Value << camera.view.far;
		out << YAML::Key << "Zoom" << YAML::Value << camera.zoom;

		out << YAML::EndMap;
	}
	void SceneSerializer::SerializeColoredQuad(YAML::Emitter& out, Entity entity, ECS::ColoredQuad& quad) {
		// Start ColoredQuad Node
		out << YAML::Key << "ColoredQuad";
		out << YAML::BeginMap;

		// Push tint
		out << YAML::Key << "Tint" << YAML::Value << quad.tint;

		out << YAML::EndMap;
	}
	void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity) {
		// Start Entity Node
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		// Push component data
		SerializeComponent<EntityMetaData>(out, entity,   &SceneSerializer::SerializeEntityMetaData, this);
		SerializeComponent<ECS::Transform>(out, entity,   &SceneSerializer::SerializeTransform,      this);
		SerializeComponent<ECS::OrthographicCamera>(out, entity, &SceneSerializer::SerializeOrthographicCamera, this);
		SerializeComponent<ECS::ColoredQuad>(out, entity, &SceneSerializer::SerializeColoredQuad,    this);

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filename) {

		LOG_MESSAGE("Serializing scene \"%s\" to file \"%s\"", 5, m_Scene->GetName().c_str(), m_Scene->m_FileName.c_str());

		// Start emitting
		YAML::Emitter out;
		out << YAML::Comment("An Enigma scene file");
		out << YAML::BeginMap;

		// Scene name
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->GetName();

		out << YAML::Key << "ActiveCamera" << YAML::Value << m_Scene->GetMetaData().activeCamera.GetUUID();

		// Push entity data
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->ForEach([&](Entity entity) {
			std::string sceneFileName = std::filesystem::path(m_Scene->m_FileName).filename().string();
			LOG_MESSAGE("Serializing entity \"%s\" to file \"%s\"", 5, entity.GetMetaData().name.c_str(), sceneFileName.c_str());
			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;

		// Stop emitting
		out << YAML::EndMap;

		// Open file
		std::ofstream file;
		file.open(filename);
		if (!file.is_open()) {
			LOG_SOFT_ERROR("Failed to save to file \"%s\"", filename.c_str());
		}

		// Save and close file
		file << out.c_str();
		file.close();
	}
	void SceneSerializer::SerializeBinary(const std::string& filename)
	{
		LOG_WARNING("function ( SceneSerializer::SerializeBinary(const std::string& filename) ) not impl yet!");
	}

	YAML::Node SceneSerializer::SerializeEntityToNode(Entity entity)
	{
		YAML::Emitter out;

		out << YAML::BeginSeq;
		for (Entity& child : entity.GetMetaData().children.GetData()) {
			SerializeEntity(out, child);
		}
		SerializeEntity(out, entity);
		out << YAML::EndSeq;

		return YAML::Load(out.c_str());
	}
	Entity SceneSerializer::DeserializeEntityFromNode(const YAML::Node& data) {

		Entity entity;

		for (YAML::Node entityData : data) {
			entity = { m_Scene->m_ECS->CreateEntity(), m_Scene.get() };

			// "Store" entity to the scene root if it doesn't have a parent
			if (!entityData["EntityMetaData"]["Parent"]) {
				EntityMetaData& sceneRoot = Entity(0, m_Scene.get()).GetComponent<EntityMetaData>();
				sceneRoot.children.Create(entity.GetID(), entity);
			}

			// Create UUID
			UUID uuid = entityData["Entity"].as<UUID>();
			entity.CreateComponent<UUID>(uuid);

			m_Scene->m_EntityUUIDs.insert({ uuid, entity });
		}

		// Load the components for each entity
		for (YAML::Node entityData : data) {
			UUID uuid = entityData["Entity"].as<UUID>();
			Entity entity = m_Scene->m_EntityUUIDs[uuid];
			DeserializeEntity(entityData, entity);
		}

		if (entity.GetMetaData().parent) {
			EntityMetaData& metaData = entity.GetMetaData().parent.GetMetaData();
			metaData.children.Create(entity.GetID(), entity);
		}

		return entity;
	}

	// Deserialization
	template<typename T, typename Func>
	static void DeserializeComponent(const std::string& name, Entity entity, const YAML::Node& data, Func func, SceneSerializer* serializer) {
		// Check if the entity has component T
		YAML::Node componentData = data[name];
		if (!componentData) return;

		// Make sure the function is valid
		if constexpr (!std::is_invocable_v<Func, SceneSerializer&, const YAML::Node&, Entity, T&>) {
			LOG_ERROR("Invalid deserialization function");
		}

		// Call deserialization function
		T& component = entity.CreateComponent<T>();
		(serializer->*func)(componentData, entity, component);
	}

	void SceneSerializer::DeserializeEntityMetaData(const YAML::Node& data, Entity entity, EntityMetaData& metaData) {
		metaData.name = data["Name"].as<std::string>();

		// Get parent entity if it exists
		if (data["Parent"]) {
			UUID parentID = data["Parent"].as<UUID>();
			metaData.parent = m_Scene->m_EntityUUIDs[parentID];
		}
		
		// Get children if they exist
		if (data["Children"]) {
			u64 childCount = 0;
			for (const YAML::Node& child : data["Children"]) {
				UUID childID = child.as<UUID>();
				Entity entity = m_Scene->m_EntityUUIDs[childID];
				metaData.children.Create(entity.GetID(), entity);
			}
		}
	}
	void SceneSerializer::DeserializeTransform(const YAML::Node& data, Entity entity, ECS::Transform& transform) {
		// Get parent entity if it exists
		if (data["Parent"]) {
			UUID uuid = data["Parent"].as<UUID>();
			transform.parent = m_Scene->m_EntityUUIDs[uuid].GetID();
		}
		
		// Deserialize transform data
		transform.position = data["Position"].as<glm::vec3>();
		transform.rotation = data["Rotation"].as<glm::quat>();
		transform.scale    = data["Scale"].as<glm::vec3>();

		// Get meta data
		TransformMetaData& metaData = entity.CreateComponent<TransformMetaData>();
		metaData.degrees     = data["Degrees"].as<bool>();
		metaData.eulerAngles = data["EulerAngles"].as<glm::vec3>();
	}
	void SceneSerializer::DeserializeOrthographicCamera(const YAML::Node& data, Entity entity, ECS::OrthographicCamera& camera) {
		camera.view.left   = data["Left"].as<f32>();
		camera.view.right  = data["Right"].as<f32>();
		camera.view.top    = data["Top"].as<f32>();
		camera.view.bottom = data["Bottom"].as<f32>();
		camera.view.near   = data["Near"].as<f32>();
		camera.view.far    = data["Far"].as<f32>();
		camera.zoom        = data["Zoom"].as<f32>();
	}
	void SceneSerializer::DeserializeColoredQuad(const YAML::Node& data, Entity entity, ECS::ColoredQuad& quad) {
		quad.tint = data["Tint"].as<glm::vec3>();
	}
	void SceneSerializer::DeserializeEntity(const YAML::Node& data, Entity entity) {
		DeserializeComponent<EntityMetaData>("EntityMetaData", entity, data, &SceneSerializer::DeserializeEntityMetaData, this);
		DeserializeComponent<ECS::Transform>("Transform",      entity, data, &SceneSerializer::DeserializeTransform,      this);
		DeserializeComponent<ECS::OrthographicCamera>("OrthographicCamera", entity, data, &SceneSerializer::DeserializeOrthographicCamera, this);
		DeserializeComponent<ECS::ColoredQuad>("ColoredQuad",  entity, data, &SceneSerializer::DeserializeColoredQuad,    this);
	}

	bool SceneSerializer::Deserialize(const std::string& filename)
	{
		// Read YAML file
		std::string fileContent = Core::System::ReadFileStr(filename);
		YAML::Node data = YAML::Load(fileContent);

		// Make sure it is a Scene file
		if (!data["Scene"]) {
			LOG_WARNING("Failed to deserialize scene \"%s\", invalid format", filename.c_str());
			return false;
		}

		m_Scene->m_FileName = filename;

		// Get Scene name
		m_Scene->GetName() = data["Scene"].as<std::string>();
		LOG_MESSAGE("Deserializing scene \"%s\" from file \"%s\"", 5, m_Scene->GetName().c_str(), filename.c_str());

		// Get scene entities
		YAML::Node entities = data["Entities"];
		if (!entities) return true;

		// Create an entity for every entity in the file
		for (YAML::Node entityData : entities) {
			Entity entity = { m_Scene->m_ECS->CreateEntity(), m_Scene.get() };

			// "Store" entity to the scene root if it doesn't have a parent
			if (!entityData["EntityMetaData"]["Parent"]) {
				EntityMetaData& sceneRoot = Entity(0, m_Scene.get()).GetComponent<EntityMetaData>();
				sceneRoot.children.Create(entity.GetID(), entity);
			}

			// Create UUID
			UUID uuid = entityData["Entity"].as<UUID>();
			entity.CreateComponent<UUID>(uuid);

			m_Scene->m_EntityUUIDs.insert({ uuid, entity });
		}

		// Load the components for each entity
		for (YAML::Node entityData : entities) {
			UUID uuid = entityData["Entity"].as<UUID>();
			Entity entity = m_Scene->m_EntityUUIDs[uuid];
			DeserializeEntity(entityData, entity);
			LOG_MESSAGE("Deserialized entity \"%s\", UUID: %s", 5, entity.GetComponent<EntityMetaData>().name.c_str(), static_cast<std::string>(uuid).c_str());
		}
		
		if (data["ActiveCamera"]) m_Scene->GetMetaData().activeCamera = m_Scene->GetEntity(data["ActiveCamera"].as<UUID>());

		return true;
	}
	bool SceneSerializer::DeserializeBinary(const std::string& filename)
	{
		LOG_WARNING("function ( SceneSerializer::DeserializeBinary(const std::string& filename) ) not impl yet!");
		return false;
	}

}