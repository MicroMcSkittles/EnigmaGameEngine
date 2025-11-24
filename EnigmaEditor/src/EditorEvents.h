#pragma once
#include <Enigma/Core/Types.h>
#include <Enigma/Core/Utilities/Utilities.h>
#include <Enigma/Core/Event/Event.h>

#include "Scene/Scene.h"
#include "Scene/Entity.h"

namespace Enigma::Editor {
	namespace EventType {
		//const u64 Save           = Core::Hash("Save");
		const u64 SceneChange    = Core::Hash("SceneChange");
		const u64 EntitySelected = Core::Hash("EntitySelected");
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
}