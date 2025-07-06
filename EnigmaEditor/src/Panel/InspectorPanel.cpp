#include "Panel/InspectorPanel.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Enigma {
	namespace Editor {

		InspectorPanel::InspectorPanel()
		{
			m_Context = new NullInspectorContext();
		}

		void InspectorPanel::ImGui()
		{
			if (m_Context == nullptr) return;

			ImGui::Begin("Inspector");

			ImGui::PushID((void*)m_Context);
			m_Context->ImGui();
			ImGui::PopID();

			ImGui::End();
		}

		void NullInspectorContext::ImGui()
		{
			ImGui::TextWrapped("Right click the hierarchy panel to get started");
		}

	}
}