#pragma once
#include <Enigma/Core/Event/Event.h>
#include <Enigma/Core/Types.h>

namespace Enigma::Editor {

	class InspectorContext {
	public:
		virtual void ShowGui() = 0;
	};

	class InspectorPanel {
	public:
		InspectorPanel() = default;

		void SetContext(ref<InspectorContext> context);
		void OnEvent(Core::Event& e);

		void ShowGui();

	private:
		ref<InspectorContext> m_Context;
	};

}