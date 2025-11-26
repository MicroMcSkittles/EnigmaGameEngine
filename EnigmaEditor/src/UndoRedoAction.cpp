#include "UndoRedoAction.h"
#include "EditorEvents.h"
#include "Scene/Components.h"
#include "Serialization/SceneSerializer.h"

#include <Enigma/Core/Process/Application.h>

namespace Enigma::Editor {
	void UndoRedoFunctions::RenameEntity(Entity entity, std::string name) {
		entity.GetMetaData().name = name;
	}
	void UndoRedoFunctions::CreateEntity(ref<Scene> scene, std::string name, Entity parent) {
		if (parent) scene->CreateEntity(parent, name);
		else scene->CreateEntity(name);
	}
	void UndoRedoFunctions::DeserializeEntity(ref<Scene> scene, YAML::Node data)
	{
		SceneSerializer serializer(scene);
		serializer.DeserializeEntityFromNode(data);
	}
	void UndoRedoFunctions::RemoveEntity(ref<Scene> scene, Engine::UUID uuid) {
		scene->RemoveEntity(scene->GetEntity(uuid));
	}
	void UndoRedoFunctions::ChangeParent(ref<Scene> scene, Entity entity, Entity parent) {
		scene->ChangeParent(entity, parent);
	}

	void RenameEntityAction(Entity entity, const std::string& newName, const std::string& oldName) {
		Action action;
		action.undoFunc = std::bind(UndoRedoFunctions::RenameEntity, entity, oldName);
		action.redoFunc = std::bind(UndoRedoFunctions::RenameEntity, entity, newName);
		action.name = "Renamed entity \"" + oldName + "\" to \"" + newName + "\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}
	void CreateEntityAction(ref<Scene> scene, Entity entity) {
		Action action;
		action.undoFunc = std::bind(UndoRedoFunctions::RemoveEntity, scene, entity.GetUUID());
		action.redoFunc = std::bind(UndoRedoFunctions::CreateEntity, scene, entity.GetMetaData().name, entity.GetMetaData().parent);
		action.name = "Created entity \"" + entity.GetMetaData().name + "\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}
	void RemoveEntityAction(ref<Scene> scene, Entity entity) {
		Action action;
		// TODO: restore components on undo
		action.undoFunc = std::bind(UndoRedoFunctions::DeserializeEntity, scene, SceneSerializer(scene).SerializeEntityToNode(entity));
		action.redoFunc = std::bind(UndoRedoFunctions::RemoveEntity, scene, entity.GetUUID());
		action.name = "Removed entity \"" + entity.GetMetaData().name + "\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}
	void ChangeParentAction(ref<Scene> scene, Entity entity, Entity parent) {
		Action action;
		action.undoFunc = std::bind(UndoRedoFunctions::ChangeParent, scene, entity, entity.GetMetaData().parent);
		action.redoFunc = std::bind(UndoRedoFunctions::ChangeParent, scene, entity, parent);
		if (parent) action.name = "Changed entity's \"" + entity.GetMetaData().name + "\" parent to \"" + parent.GetMetaData().name + "\"";
		else action.name = "Changed entity's \"" + entity.GetMetaData().name + "\" parent to \"Root\"";

		Event::NewAction e(action);
		Core::Application::EventCallback(e);
	}

	ActionHandler::ActionHandler()
	{
		m_ActionPointer = InvalidActionPointer;
	}
	void ActionHandler::OnEvent(Core::Event& e)
	{
		Core::EventHandler handler(e);
		handler.Dispatch<Event::Undo>([&](Event::Undo& e) { Undo(); return false; });
		handler.Dispatch<Event::Redo>([&](Event::Redo& e) { Redo(); return false; });
		handler.Dispatch<Event::NewAction>([&](Event::NewAction& e) { NewAction(e.GetAction()); return false; });
	}

	void ActionHandler::Undo()
	{
		// Get action to undo
		Action& action = m_Actions[m_ActionPointer--];
		action.undoFunc();
		LOG_MESSAGE("Undoing action ( %s )", 5, action.name.c_str());
	}
	void ActionHandler::Redo()
	{
		// Get action to redo
		Action& action = m_Actions[++m_ActionPointer];
		action.redoFunc();
		LOG_MESSAGE("Redoing action ( %s )", 5, action.name.c_str());
	}

	bool ActionHandler::CanUndo()
	{
		return m_ActionPointer >= 0 && m_ActionPointer < m_Actions.size();
	}
	bool ActionHandler::CanRedo()
	{
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

		LOG_MESSAGE("New Action ( %s )", 5, action.name.c_str());
	}

}