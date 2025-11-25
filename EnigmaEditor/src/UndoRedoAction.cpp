#include "UndoRedoAction.h"
#include "EditorEvents.h"

namespace Enigma::Editor {
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