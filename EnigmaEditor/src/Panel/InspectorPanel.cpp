#include "Panel/InspectorPanel.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Enigma {
	namespace Editor {

		void InspectorPanel::ImGui()
		{
			if (m_Context == nullptr) return;

			ImGui::Begin("Inspector");

			ImGui::PushID((void*)m_Context);
			m_Context->ImGui();
			ImGui::PopID();

			ImGui::End();
		}

	}
}