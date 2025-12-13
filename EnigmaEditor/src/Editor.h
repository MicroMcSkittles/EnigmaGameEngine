#pragma once
#include <Enigma/Core/Process/SubProcess.h>

#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/SceneViewPanel.h"

#include "UndoRedoAction.h"

namespace Enigma::Editor {

	enum EditorState {
		EditorState_Editing = 0,
		EditorState_Running
	};

	class EditorProcess : public Core::SubProcess {
	public:
		virtual void StartUp() override;
		virtual void ShutDown() override;

		virtual bool OnEvent(Core::Event& e) override;

		virtual void Update(Engine::DeltaTime deltaTime) override;
		virtual void Render() override;
		virtual void ImGui() override;

	private:
		void StartRuntime();
		void PauseRuntime();

		void MainMenuBar();
		void MainMenuBarFile();
		void MainMenuBarEdit();
		void MainMenuBarHelp();

		void SaveActiveScene(bool dialog = false);
		void OpenScene();

	private:
		Core::ID m_WindowID;

		u64 m_EditorState;
		ref<Scene> m_ActiveScene;
		ref<Scene> m_EditorScene;
		ref<Scene> m_RuntimeScene;
		Entity m_Entity;

		unique<ActionHandler> m_EditorActionHandler;
		unique<ActionHandler> m_RuntimeActionHandler;

		unique<SceneHierachyPanel> m_SceneHierachyPanel;
		unique<SceneViewPanel> m_SceneViewPanel;
		unique<InspectorPanel> m_InspectorPanel;
	};	
}
