#pragma once

namespace Enigma {
	namespace Editor {

		class InspectorContext {
		public:
			virtual ~InspectorContext() { }
			virtual void ImGui() = 0;
		};

		class NullInspectorContext : public InspectorContext {
		public:
			virtual void ImGui() override;
		};

		class InspectorPanel {
		public:
			InspectorPanel();

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