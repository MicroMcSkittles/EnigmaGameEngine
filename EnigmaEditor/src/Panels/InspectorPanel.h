#pragma once
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

		void ShowGui();

	private:
		ref<InspectorContext> m_Context;
	};

}