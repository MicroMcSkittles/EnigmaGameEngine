#pragma once
#include <Enigma/Core/Types.h>
#include <Enigma/Core/Utilities/Utilities.h>
#include <Enigma/Core/Event/Event.h>

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "UndoRedoAction.h"
#include "Editor.h"

#include "Panels/InspectorPanel.h"

#include <functional>

namespace Enigma::Editor::Event {
	namespace EventType {
		//const u64 Save           = Core::Hash("Save");
		const u64 SceneChange         = Core::Hash("SceneChange");
		const u64 EntitySelected      = Core::Hash("EntitySelected");
		const u64 NewInspectorContext = Core::Hash("NewInspectorContext");
		const u64 NewAction           = Core::Hash("NewAction");
		const u64 Undo                = Core::Hash("Undo");
		const u64 Redo                = Core::Hash("Redo");
		const u64 StartRuntime        = Core::Hash("StartRuntime");
		const u64 PauseRuntime        = Core::Hash("PauseRuntime");
	}

	/*class Save : public Core::Event {
	public:
		EVENT_TYPE(Save);
		EVENT_CATEGORY(Core::EventCategory::EventNone);

		Save() { }

		virtual std::string ToString() override {
			return "Save Event";
		}
	};*/

	class StartRuntime : public Core::Event {
	public:
		EVENT_TYPE(StartRuntime);
		EVENT_CATEGORY(Core::EventCategory::EventNone);

		virtual std::string ToString() override {
			return "Start Runtime Event";
		}

	};
	class PauseRuntime : public Core::Event {
	public:
		EVENT_TYPE(PauseRuntime);
		EVENT_CATEGORY(Core::EventCategory::EventNone);

		virtual std::string ToString() override {
			return "Pause Runtime Event";
		}
	};

	class SceneChange : public Core::Event {
	public:
		EVENT_TYPE(SceneChange);
		EVENT_CATEGORY(Core::EventCategory::EventNone);

		SceneChange(const ref<Scene>& scene): m_Scene(scene) { }

		virtual std::string ToString() override {
			return "Scene Change Event";
		}

		ref<Scene> GetScene() { return m_Scene; }

	private:
		ref<Scene> m_Scene;
	};

	class EntitySelected : public Core::Event {
	public:
		EVENT_TYPE(EntitySelected);
		EVENT_CATEGORY(Core::EventCategory::EventNone);

		EntitySelected(Entity entity): m_Entity(entity) { }

		virtual std::string ToString() override {
			return "Entity Selected Event";
		}

		Entity GetEntity() { return m_Entity; }

	private:
		Entity m_Entity;
	};
	class NewInspectorContext : public Core::Event {
	public:
		EVENT_TYPE(NewInspectorContext);
		EVENT_CATEGORY(Core::EventCategory::EventNone);

		NewInspectorContext(ref<InspectorContext> context) : m_Context(context) { }

		virtual std::string ToString() override {
			return "New Inspector Context";
		}

		ref<InspectorContext> GetContext() { return m_Context; }

	private:
		ref<InspectorContext> m_Context;
	};

	// Undo/Redo events

	// Undoes the last recorded action
	class Undo : public Core::Event {
	public:
		EVENT_TYPE(Undo);
		EVENT_CATEGORY(Core::EventCategory::EventNone);

		Undo() { }
	};

	// Redoes the last recorded action that was undone
	class Redo : public Core::Event {
	public:
		EVENT_TYPE(Redo);
		EVENT_CATEGORY(Core::EventCategory::EventNone);

		Redo() { }
	};

	// A action that can happen
	class NewAction : public Core::Event {
	public:
		EVENT_TYPE(NewAction);
		EVENT_CATEGORY(Core::EventCategory::EventNone);

		NewAction(const Action& action): m_Action(action) { }

		Action GetAction() { return m_Action; }

	private:
		Action m_Action;
	};
}