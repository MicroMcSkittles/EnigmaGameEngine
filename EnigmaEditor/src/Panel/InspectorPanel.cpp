#include "Panel/InspectorPanel.h"

#include <Enigma/Core/System.h>
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

			ImGui::Begin("Inspector");

			if (m_Context != nullptr) m_Context->ImGui();

			ImGui::End();
		}

		void NullInspectorContext::ImGui()
		{
			ImGui::TextWrapped("Right click the hierarchy panel to get started");
		}

		TextFileInspectorContext::TextFileInspectorContext(const std::string& filename)
		{
			m_Filename = filename;
			m_Content = Core::System::ReadFileStr(m_Filename);
		}
		void TextFileInspectorContext::ImGui()
		{
			ImGui::Text(m_Filename.c_str());
			ImGui::BeginChild("TEXT_FILE_INSPECTOR_CONTEXT_CONTENT", {0,0}, ImGuiChildFlags_Border);
			ImGui::TextWrapped(m_Content.c_str());
			ImGui::EndChild();
		}

	}
}