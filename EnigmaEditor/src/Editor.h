#pragma once
#include <Enigma/Core/Process/SubProcess.h>
#include <Enigma/Core/Event/Event.h>		
#include <Enigma/Core/Event/WindowEvent.h>
#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Core/IdHandler.h>

#include "Panel/HierarchyPanel.h"
#include "Panel/InspectorPanel.h"
#include "Panel/LoggerPanel.h"
#include "Panel/SceneViewPanel.h"
#include "Panel/ContentBrowserPanel.h"
#include "Scene/SceneView.h"
#include "EditorEvent.h"
#include "Serialization/Project.h"

namespace Enigma {
	namespace Editor {

		class Editor : public Core::SubProcess {
		public:
			static Editor* Get() { return s_Instance; }
			static Project* GetActiveProject() { return s_Instance->m_CurrentProject; }

			virtual void StartUp() override;
			virtual void ShutDown() override;

			bool OnWindowClose(Core::WindowClose& e);
			virtual bool OnEvent(Core::Event& e) override;

			virtual void ImGui() override;

		private:
			bool OnKeyboard(Core::Keyboard& e);
			void OnSave();
			void OnInspectorContext(NewInspectorContext& e);
			void CreateWindowID();

		private:
			Core::ID m_WindowID;

			HierarchyPanel* m_HierarchyPanel;
			InspectorPanel* m_InspectorPanel;
			LoggerPanel* m_LoggerPanel;
			SceneViewPanel* m_SceneViewPanel;
			SceneView2D* m_SceneView;
			ContentBrowserPanel* m_ContentBrowserPanel;

			Project* m_CurrentProject;

		private:
			inline static Editor* s_Instance;
		};

	}
}