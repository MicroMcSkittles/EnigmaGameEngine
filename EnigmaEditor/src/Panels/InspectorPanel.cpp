#include "Panels/InspectorPanel.h"

#include <imgui.h>

namespace Enigma::Editor {
	void InspectorPanel::SetContext(ref<InspectorContext> context)
	{
		m_Context = context;
	}
	void InspectorPanel::ShowGui()
	{
		ImGui::Begin("Inspector");

		if (m_Context == nullptr) {
			ImGui::TextWrapped("No inspector context");
			ImGui::End();
			return;
		}

		m_Context->ShowGui();

		ImGui::End();
	}
}