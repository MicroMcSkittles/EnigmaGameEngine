#pragma once
#include <Enigma/Core/Event/Event.h>
#include <functional>
#include <string>

#include <yaml-cpp/yaml.h>

#include "Scene/Scene.h"
#include "Scene/Entity.h"

namespace Enigma::Editor {

	namespace UndoRedoFunctions {
		void RenameEntity(ref<Scene> scene, Engine::UUID entityUUID, std::string name);
		void CreateEntity(ref<Scene> scene, std::string name, Engine::UUID entityUUID, Engine::UUID parentUUID);
		void DeserializeEntity(ref<Scene> scene, YAML::Node data);
		void RemoveEntity(ref<Scene> scene, Engine::UUID entityUUID);
		void ChangeParentRoot(ref<Scene> scene, Engine::UUID entityUUID);
		void ChangeParent(ref<Scene> scene, Engine::UUID entityUUID, Engine::UUID parentUUID);
	}
	void RenameEntityAction(ref<Scene> scene, Entity entity, const std::string& newName, const std::string& oldName);
	void CreateEntityAction(ref<Scene> scene, Entity entity);
	void RemoveEntityAction(ref<Scene> scene, Entity entity);
	void ChangeParentAction(ref<Scene> scene, Entity entity, Entity parent);

	struct Action {
		std::function<void()> undoFunc; // undoes the action
		std::function<void()> redoFunc; // redoes the action
		std::string name;

		Action() = default;
		Action(const Action& other) : undoFunc(other.undoFunc), redoFunc(other.redoFunc), name(other.name) { }
	};

	class ActionHandler {
	public:
		ActionHandler();
		~ActionHandler() = default;

		void OnEvent(Core::Event& e);

		void Undo();
		void Redo();

		bool CanUndo();
		bool CanRedo();

	private:
		void NewAction(const Action& action);

	private:
		std::vector<Action> m_Actions;
		i64 m_ActionPointer;

		const i64 InvalidActionPointer = -1;
	};

}