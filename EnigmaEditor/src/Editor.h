#pragma once
#include <Enigma/Core/Process/SubProcess.h>

#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/SceneViewPanel.h"

namespace Enigma::Editor {
	class EditorProcess : public Core::SubProcess {
	public:
		virtual void StartUp() override;
		virtual void ShutDown() override;

		virtual bool OnEvent(Core::Event& e) override;

		virtual void Update(Engine::DeltaTime deltaTime) override;
		virtual void Render() override;
		virtual void ImGui() override;

	private:
		void MainMenuBar();
		void SaveActiveScene();

	private:
		Core::ID m_WindowID;

		ref<Scene> m_ActiveScene;
		Entity m_Entity;

		unique<SceneHierachyPanel> m_SceneHierachyPanel;
		unique<SceneViewPanel> m_SceneViewPanel;
		unique<InspectorPanel> m_InspectorPanel;
	};	
}
