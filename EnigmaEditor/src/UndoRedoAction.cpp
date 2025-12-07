#include "UndoRedoAction.h"
#include "EditorEvents.h"
#include "Scene/Components.h"
#include "Serialization/SceneSerializer.h"

#include <Enigma/Core/Process/Application.h>

namespace Enigma::Editor {
	void UndoRedoFunctions::RenameEntity(ref<Scene> scene, Engine::UUID entityUUID, std::string name) {
		scene->GetEntity(entityUUID).GetMetaData().name = name;
	}
	void UndoRedoFunctions::RenameScene(ref<Scene> scene, std::string name) {
		scene->GetName() = name;
	}
	void UndoRedoFunctions::CreateEntity(ref<Scene> scene, std::string name, Engine::UUID entityUUID, Engine::UUID parentUUID) {
		Entity parent = scene->GetEntity(parentUUID);
		if (parent) scene->CreateEntity(parent, name, entityUUID);
		else scene->CreateEntity(name, entityUUID);
	}
	void UndoRedoFunctions::DeserializeEntity(ref<Scene> scene, YAML::Node data)
	{
		SceneSerializer serializer(scene);
		serializer.DeserializeEntityFromNode(data);
	}
	void UndoRedoFunctions::RemoveEntity(ref<Scene> scene, Engine::UUID entityUUID) {
		scene->RemoveEntity(scene->GetEntity(entityUUID));
	}
	void UndoRedoFunctions::ChangeParentRoot(ref<Scene> scene, Engine::UUID entityUUID)
	{
		scene->ChangeParent(scene->GetEntity(entityUUID), {});
	}
	void UndoRedoFunctions::ChangeParent(ref<Scene> scene, Engine::UUID entityUUID, Engine::UUID parentUUID) {
		scene->ChangeParent(scene->GetEntity(entityUUID), scene->GetEntity(parentUUID));
	}

	void RenameEntityAction(const ref<Scene>& scene, Entity entity, const std::string& newName, const std::string& oldName) {
		Action action;
		action.undoFunc = std::bind(UndoRedoFunctions::RenameEntity, scene, entity.GetUUID(), oldName);
		action.redoFunc = std::bind(UndoRedoFunctions::RenameEntity, scene, entity.GetUUID(), newName);
		action.name = "Renamed entity \"" + oldName + "\" to \"" + newName + "\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}
	void RenameSceneAction(const ref<Scene>& scene, const std::string& newName, const std::string& oldName) {
		Action action;
		action.undoFunc = std::bind(UndoRedoFunctions::RenameScene, scene, oldName);
		action.redoFunc = std::bind(UndoRedoFunctions::RenameScene, scene, newName);
		action.name = "Renamed scene \"" + oldName + "\" to \"" + newName + "\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}
	void CreateEntityAction(const ref<Scene>& scene, Entity entity) {
		Action action;
		action.undoFunc = std::bind(UndoRedoFunctions::RemoveEntity, scene, entity.GetUUID());
		Engine::UUID parentUUID = (entity.GetMetaData().parent) ? entity.GetMetaData().parent.GetUUID() : 0;
		action.redoFunc = std::bind(UndoRedoFunctions::CreateEntity, scene, entity.GetMetaData().name, entity.GetUUID(), parentUUID);
		action.name = "Created entity \"" + entity.GetMetaData().name + "\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}
	void RemoveEntityAction(const ref<Scene>& scene, Entity entity) {
		Action action;
		// TODO: restore components on undo
		action.undoFunc = std::bind(UndoRedoFunctions::DeserializeEntity, scene, SceneSerializer(scene).SerializeEntityToNode(entity));
		action.redoFunc = std::bind(UndoRedoFunctions::RemoveEntity, scene, entity.GetUUID());
		action.name = "Removed entity \"" + entity.GetMetaData().name + "\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}
	void ChangeParentAction(const ref<Scene>& scene, Entity entity, Entity parent) {

		Action action;
		if (entity.GetMetaData().parent)
			action.undoFunc = std::bind(UndoRedoFunctions::ChangeParent, scene, entity.GetUUID(), entity.GetMetaData().parent.GetUUID());
		else action.undoFunc = std::bind(UndoRedoFunctions::ChangeParentRoot, scene, entity.GetUUID());
		if (parent)
			action.redoFunc = std::bind(UndoRedoFunctions::ChangeParent, scene, entity.GetUUID(), parent.GetUUID());
		else action.redoFunc = std::bind(UndoRedoFunctions::ChangeParentRoot, scene, entity.GetUUID());
		
		if (parent) action.name = "Changed entity's \"" + entity.GetMetaData().name + "\" parent to \"" + parent.GetMetaData().name + "\"";
		else action.name = "Changed entity's \"" + entity.GetMetaData().name + "\" parent to \"Root\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}

	ActionHandler::ActionHandler() {
		m_ActionPointer = InvalidActionPointer;
		m_ActionDepth = 50;
	}
	void ActionHandler::OnEvent(Core::Event& e) {
		Core::EventHandler handler(e);
		handler.Dispatch<Event::Undo>([&](Event::Undo& e) { Undo(); return false; });
		handler.Dispatch<Event::Redo>([&](Event::Redo& e) { Redo(); return false; });
		handler.Dispatch<Event::NewAction>([&](Event::NewAction& e) { NewAction(e.GetAction()); return false; });
	}

	void ActionHandler::Undo() {
		// Get action to undo
		Action& action = m_Actions[m_ActionPointer--];
		action.undoFunc();
		LOG_MESSAGE("Undoing action ( %s )", 5, action.name.c_str());
	}
	void ActionHandler::Redo() {
		// Get action to redo
		Action& action = m_Actions[++m_ActionPointer];
		action.redoFunc();
		LOG_MESSAGE("Redoing action ( %s )", 5, action.name.c_str());
	}

	bool ActionHandler::CanUndo() {
		return m_ActionPointer >= 0 && m_ActionPointer < m_Actions.size();
	}
	bool ActionHandler::CanRedo() {
		return m_ActionPointer < static_cast<i64>(m_Actions.size()) - 1;
	}

	void ActionHandler::NewAction(const Action& action)
	{
		if (m_Actions.empty() || m_ActionPointer == static_cast<i64>(m_Actions.size()) - 1) {
			m_Actions.push_back(action);
		}
		else if (m_ActionPointer < static_cast<i64>(m_Actions.size())) {
			m_Actions.resize(m_ActionPointer + 2);
			m_Actions[m_ActionPointer + 1] = action;
		}
		m_ActionPointer += 1;

		// Shift actions back if the list is to large
		if (m_Actions.size() > m_ActionDepth) {
			m_Actions = std::vector<Action>(m_Actions.begin() + 1, m_Actions.end());
			m_ActionPointer -= 1;
		}

		LOG_MESSAGE("New Action ( %s )", 5, action.name.c_str());
	}

}