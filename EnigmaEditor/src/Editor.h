#pragma once
#include <Enigma/Core/Process/SubProcess.h>
#include <Enigma/Core/Event/Event.h>		
#include <Enigma/Core/Event/WindowEvent.h>		
#include <Enigma/Core/IdHandler.h>

#include "Panel/HierarchyPanel.h"
#include "Panel/InspectorPanel.h"
#include "Panel/LoggerPanel.h"
#include "Panel/SceneViewPanel.h"
#include "Scene/Scene.h"
#include "Scene/SceneView.h"

namespace Enigma {
	namespace Editor {

		class Editor : public Core::SubProcess {
		public:
			virtual void StartUp() override;
			virtual void ShutDown() override;

			bool OnWindowClose(Core::WindowClose& e);
			virtual bool OnEvent(Core::Event& e) override;

			virtual void ImGui() override;
		private:
			void CreateWindowID();

		private:
			Core::ID m_WindowID;

			HierarchyPanel* m_HierarchyPanel;
			InspectorPanel* m_InspectorPanel;
			LoggerPanel* m_LoggerPanel;
			SceneViewPanel* m_SceneViewPanel;
			SceneView2D* m_SceneView;
			Scene* m_Scene;
		};

	}
}