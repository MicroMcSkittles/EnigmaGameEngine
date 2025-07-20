#pragma once
#include <Enigma/Core/Event/Event.h>

#include "Panel/InspectorPanel.h"

namespace Enigma {
	namespace Editor {

		namespace EventType {
			const uint64_t ProjectSave = Core::Hash("ProjectSave");
			const uint64_t NewInspectorContext = Core::Hash("NewInspectorContext");
		}

		enum EditorEventCategory {
			EditorEventLastBit       = Core::LastEventCategoryBit,
			EditorEventSerialization = BIT(EditorEventLastBit + 1),
			EditorEventInspector     = BIT(EditorEventLastBit + 2),
		};

		class ProjectSave : public Core::Event {
		public:
			EVENT_TYPE(ProjectSave);
			EVENT_CATEGORY((uint32_t)EditorEventCategory::EditorEventSerialization);

			ProjectSave() { }

			virtual std::string ToString() override {
				return "Save Project";
			}
		};
		class NewInspectorContext : public Core::Event {
		public:
			EVENT_TYPE(NewInspectorContext);
			EVENT_CATEGORY((uint32_t)EditorEventCategory::EditorEventInspector);

			NewInspectorContext(InspectorContext* context) : m_Context(context) { }

			virtual std::string ToString() override {
				return "New Inspector Context";
			}

			InspectorContext* GetContext() {
				return m_Context;
			}

		private:
			InspectorContext* m_Context;
		};

	}
}