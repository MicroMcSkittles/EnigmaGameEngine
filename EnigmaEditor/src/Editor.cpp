#include "Editor.h"

#include <Enigma/Core/Core.h>
#include <Enigma/Core/Window.h>
#include <Enigma/Core/Process/Application.h>

#include <Enigma/Engine/ECS/RenderComponent.h>

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
			// Setup Logger
			m_LoggerPanel = new LoggerPanel();
			SET_LOG_CALLBACK([&](const std::string& message, const Core::LogInfo& info) {
				m_LoggerPanel->Log(message, info);
			});

			// Setup Scene stuff
			m_Scene = new Scene();
			m_InspectorPanel = new InspectorPanel();
			m_HierarchyPanel = new HierarchyPanel([&](Entity* entity) {
				m_InspectorPanel->SetContext(new EntityInspectorContext(entity));
			});
			m_HierarchyPanel->SetContext(m_Scene);
			
			// Create Window
			CreateWindowID();

			Core::Application::BindSubProcToWindow(this, m_WindowID);
			Core::Application::GetWindow(m_WindowID)->AddEventCallback([&](Core::Event& e) {OnEvent(e); });

			// Setup scene view
			m_SceneView = Core::Application::CreateSubProc<SceneView2D>();
			Core::Application::BindSubProcToWindow(m_SceneView, m_WindowID);
			m_SceneView->SetWindowContext(Core::Application::GetWindow(m_WindowID));
			m_SceneView->SetSceneContext(m_Scene);

			m_SceneViewPanel = new SceneViewPanel(m_SceneView);

			LOG_WARNING("Starting Editor");
			LOG_SOFT_ERROR("Test Error");
		}
		void Editor::ShutDown()
		{
			LOG_WARNING("Shutting Down Editor");
		}

		bool Editor::OnWindowClose(Core::WindowClose& e)
		{
			Core::Application::Close();
			return false;
		}
		bool Editor::OnEvent(Core::Event& e)
		{
			Core::EventHandler handler(e);
			handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& e) {return OnWindowClose(e); });
			return false;
		}

		void Editor::ImGui()
		{
			Core::Application::UseRenderAPI(Renderer::API::OpenGL);
			//Renderer::RenderAPI::SetClearMask(Renderer::ColorBufferBit);
			Renderer::RenderAPI::Clear();
			
			m_LoggerPanel->ImGui();
			m_HierarchyPanel->ImGui();
			m_InspectorPanel->ImGui();
			m_SceneViewPanel->ImGui();
		}
		
	}
}