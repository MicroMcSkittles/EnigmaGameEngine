#include "Editor.h"

#include <Enigma/Core/Core.h>
#include <Enigma/Core/Window.h>
#include <Enigma/Core/Process/Application.h>

#include <Enigma/Engine/ECS/RenderComponent.h>
#include <Enigma/Engine/InputCodes.h>

#include <imgui.h>

#include "Serialization/JSON.h"
#include "Serialization/Image.h"
#include "EditorEvent.h"
#include "EditorIcons.h"

namespace Enigma {
	namespace Editor {
		void Editor::CreateWindowID()
		{
			Core::WindowConfig windowConfig;
			windowConfig.width = 1024;
			windowConfig.height = 720;
			windowConfig.renderAPI = Renderer::API::OpenGL;
			windowConfig.title = "Enigma Editor";

			Core::ImGuiConfig imguiConfig;
			imguiConfig.docking = true;

			m_WindowID = Core::Application::CreateWindow(windowConfig, imguiConfig);
		}
		void Editor::StartUp()
		{
			if (s_Instance) {
				LOG_SOFT_ERROR("Editor instance already exists");
				return;
			}
			s_Instance = this;

			// Setup Logger
			m_LoggerPanel = new LoggerPanel();
			SET_LOG_CALLBACK([&](const std::string& message, const Core::LogInfo& info) {
				m_LoggerPanel->Log(message, info);
			});

			// Setup Scene stuff
			m_CurrentProject = new Project("ExampleProject\\.Enigma\\proj.json");
			m_CurrentProject->LoadScene();

			m_InspectorPanel = new InspectorPanel();
			m_HierarchyPanel = new HierarchyPanel();
			m_HierarchyPanel->SetContext(m_CurrentProject->GetActiveScene());
			
			// Create Window
			CreateWindowID();

			Core::Application::BindSubProcToWindow(this, m_WindowID);
			Core::Application::GetWindow(m_WindowID)->AddEventCallback([&](Core::Event& e) {OnEvent(e); });

			Core::Application::UseRenderAPI(Renderer::API::OpenGL);
			Icons::Load();

			// Setup scene view
			m_SceneView = Core::Application::CreateSubProc<SceneView2D>();
			m_SceneView->SetWindowContext(m_WindowID);
			m_SceneView->SetSceneContext(m_CurrentProject->GetActiveScene());

			m_SceneViewPanel = new SceneViewPanel(m_SceneView);

			m_ContentBrowserPanel = new ContentBrowserPanel();

			LOG_WARNING("Starting Editor");
			LOG_SOFT_ERROR("Test Error");
		}
		void Editor::ShutDown()
		{
			delete m_CurrentProject;
			LOG_WARNING("Shutting Down Editor");
		}

		void Editor::OnSave()
		{
			m_CurrentProject->Save();
		}

		void Editor::OnInspectorContext(NewInspectorContext& e)
		{
			m_InspectorPanel->SetContext(e.GetContext());
		}

		bool Editor::OnKeyboard(Core::Keyboard& e)
		{
			if (e.GetAction() != Engine::KeyCode::KeyPress) return false;
			if (e.GetKey() != Engine::KeyCode::KeyS) return false;
			if (!(e.GetMods() & Engine::KeyCode::CtrlMask)) return false;

			ProjectSave saveEvent;
			Core::Application::EventCallback(saveEvent);

			return false;
		}
		bool Editor::OnWindowClose(Core::WindowClose& e)
		{
			ProjectSave saveEvent;
			Core::Application::EventCallback(saveEvent);

			Core::Application::Close();
			return false;
		}
		bool Editor::OnEvent(Core::Event& e)
		{
			Core::EventHandler handler(e);
			handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& e) {return OnWindowClose(e); });
			handler.Dispatch<Core::Keyboard>([&](Core::Keyboard& e) {return OnKeyboard(e); });
			handler.Dispatch<ProjectSave>([&](ProjectSave& e) { OnSave(); return false; });
			handler.Dispatch<NewInspectorContext>([&](NewInspectorContext& e) { OnInspectorContext(e); return false; });
			return false;
		}

		void Editor::ImGui()
		{
			Core::Application::UseRenderAPI(Renderer::API::OpenGL);
			Renderer::RenderAPI::Clear();
			
			/*if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("File")) {

					if (ImGui::MenuItem("Save","CTRL+S")) {
						ProjectSave saveEvent;
						Core::Application::EventCallback(saveEvent);
					}

					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}*/

			m_LoggerPanel->ImGui();
			m_HierarchyPanel->ImGui();
			m_InspectorPanel->ImGui();
			m_SceneViewPanel->ImGui();
			m_ContentBrowserPanel->ImGui();
		}
	}
}