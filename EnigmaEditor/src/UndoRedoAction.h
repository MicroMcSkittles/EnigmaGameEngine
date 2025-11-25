#pragma once
#include <Enigma/Core/Event/Event.h>
#include <functional>
#include <string>

namespace Enigma::Editor {

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