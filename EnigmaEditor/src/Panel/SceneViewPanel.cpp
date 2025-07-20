#include "Panel/SceneViewPanel.h"

#include <Enigma/ImGui/EnigmaWidgets.h>
#include <imgui.h>

namespace Enigma {
	namespace Editor {
		SceneViewPanel::SceneViewPanel(SceneView2D* sceneView)
		{
			m_SceneView = sceneView;
			m_LastViewSize = { 0,0 };
		}
		SceneViewPanel::~SceneViewPanel()
		{
		}

		void SceneViewPanel::ImGui()
		{
			ImGui::Begin("Scene View");

			Engine::Surface surface;
			surface.position = {
				ImGui::GetCursorScreenPos().x,
				ImGui::GetCursorScreenPos().y
			};
			surface.scale = {
				ImGui::GetContentRegionAvail().x,
				ImGui::GetContentRegionAvail().y
			};
			if (m_LastViewSize != surface.scale) {
				m_SceneView->Resize(surface.scale.x, surface.scale.y);
				m_LastViewSize = surface.scale;
			}
			m_SceneView->SetSurface(surface);
			
			ImGui::Image(m_SceneView->GetFrame());
			ImGui::End();
		}
	}
}