#pragma once

namespace Enigma {
	namespace Editor {

		class InspectorContext {
		public:
			virtual ~InspectorContext() { }
			virtual void ImGui() = 0;
		};

		class InspectorPanel {
		public:
			InspectorPanel() : m_Context(nullptr) { }

			void SetContext(InspectorContext* context) {
				if (m_Context) delete m_Context;
				m_Context = context; 
			}

			void ImGui();

		private:
			InspectorContext* m_Context;
		};

	}
}