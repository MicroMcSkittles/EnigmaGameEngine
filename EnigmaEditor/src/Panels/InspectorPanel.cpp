#include "Panels/InspectorPanel.h"
#include "EditorEvents.h"

#include <imgui.h>

namespace Enigma::Editor {
	void InspectorPanel::SetContext(ref<InspectorContext> context)
	{
		m_Context = context;
	}
	void InspectorPanel::OnEvent(Core::Event& e)
	{
		Core::EventHandler handler(e);
		handler.Dispatch<Event::SceneChange>([&](Event::SceneChange& e) {
			SetContext(nullptr);
			return false;
		});
		handler.Dispatch<Event::NewInspectorContext>([&](Event::NewInspectorContext& e) {
			SetContext(e.GetContext());
			return false;
		});
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