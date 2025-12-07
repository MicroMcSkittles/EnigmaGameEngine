#include "Editor.h"
#include "EditorImGui.h"
#include "EditorEvents.h"
#include "Serialization/SceneSerializer.h"

#include <Enigma/Core/System.h>
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Event/WindowEvent.h>

#include <Enigma/Engine/ECS/Components.h>

#include <filesystem>

#include <imgui.h>
#include <ImGuizmo.h>

#include <Enigma/Core/Utilities/Utilities.h>

namespace Enigma::Editor {
	void EditorProcess::StartUp()
	{
		SET_LOGGER_FLAGS(Core::LoggerTime | Core::LoggerShort);

		// Create Window
		Core::WindowConfig windowConfig;
		windowConfig.title     = "Enigma Editor";
		windowConfig.width     = 1024;
		windowConfig.height    = 720;
		windowConfig.renderAPI = Renderer::API::OpenGL;

		Core::ImGuiConfig imguiConfig;
		imguiConfig.renderAPI = windowConfig.renderAPI;
		imguiConfig.docking = true;

		m_WindowID = Core::Application::CreateWindow(windowConfig, imguiConfig);
		Core::Application::GetWindow(m_WindowID)->AddEventCallback([&](Core::Event& e) { return OnEvent(e); });
		Core::Application::BindSubProcToWindow(Core::Application::GetSubProcess(m_ProcessID), m_WindowID);

		// Init RenderAPI
		Core::Application::UseRenderAPI(windowConfig.renderAPI);
		Renderer::RenderAPI::SetClearMask(Renderer::ColorBufferBit);
		Renderer::RenderAPI::SetClearColor({ 0,0,0,1 });

		// Init Gui
		EditorStyle style;
		EditorGui::SetStyle(style);

		// Load Scene
		m_ActiveScene = Scene::Create();
		SceneSerializer serializer(m_ActiveScene);
		serializer.Deserialize("Scene.scene");

		// Create action handler
		m_ActionHandler = CreateUnique<ActionHandler>();

		// Configure Inspector
		m_InspectorPanel = CreateUnique<InspectorPanel>();

		// Configure Scene View
		m_SceneViewPanel = CreateUnique<SceneViewPanel>(m_WindowID);
		m_SceneViewPanel->SetContext(m_ActiveScene);

		// Configure Scene Hierachy
		m_SceneHierachyPanel = CreateUnique<SceneHierachyPanel>();
		m_SceneHierachyPanel->SetContext(m_ActiveScene);
	}
	void EditorProcess::ShutDown()
	{
	}

	bool EditorProcess::OnEvent(Core::Event& e)
	{
		Core::EventHandler handler(e);

		// Stop the application when the window closes
		handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& windowEvent) {
			Core::Application::Close();
			return false;
		});

		handler.Dispatch<Event::EntitySelected>([&](Event::EntitySelected& e) {
			//m_InspectorPanel->SetContext(EntityInspectorContext::Create(e.GetEntity(), m_ActiveScene));
			Event::NewInspectorContext contextEvent(EntityInspectorContext::Create(e.GetEntity(), m_ActiveScene));
			Core::Application::EventCallback(contextEvent);
			return false; 
		});

		m_ActionHandler->OnEvent(e);

		m_SceneViewPanel->OnEvent(e);
		m_SceneHierachyPanel->OnEvent(e);
		m_InspectorPanel->OnEvent(e);

		return false;
	}

	void EditorProcess::Update(Engine::DeltaTime deltaTime)
	{
		m_SceneViewPanel->Update(deltaTime);
	}
	void EditorProcess::Render()
	{
		Renderer::RenderAPI::Clear();
		m_SceneViewPanel->Render();
	}

	void EditorProcess::ImGui()
	{
		ImGuizmo::BeginFrame();

		MainMenuBar();
		Core::ImGuiHandler::DockSpace(0, ImGui::GetFrameHeight(), 0, -ImGui::GetFrameHeight());

		// TODO: add custom key binds
		// Shortcuts
		ImGuiInputFlags shortcutFlags = ImGuiInputFlags_RouteAlways;

		// Undo/Redo
		if (ImGui::Shortcut(ImGuiKey_Z | ImGuiMod_Ctrl, shortcutFlags) && m_ActionHandler->CanUndo()) m_ActionHandler->Undo();
		if (ImGui::Shortcut(ImGuiKey_Y | ImGuiMod_Ctrl, shortcutFlags) && m_ActionHandler->CanRedo()) m_ActionHandler->Redo();

		// File
		if (ImGui::Shortcut(ImGuiKey_S | ImGuiMod_Ctrl, shortcutFlags)) SaveActiveScene();
		if (ImGui::Shortcut(ImGuiKey_O | ImGuiMod_Ctrl, shortcutFlags)) OpenScene();

		m_InspectorPanel->ShowGui();
		m_SceneHierachyPanel->ShowGui();
		m_SceneViewPanel->ShowGui();
	}
	void EditorProcess::MainMenuBar()
	{
		if (!ImGui::BeginMainMenuBar()) return;

		// Show tabs
		MainMenuBarFile();
		MainMenuBarEdit();
		MainMenuBarHelp();

		ImGui::EndMainMenuBar();
	}

	void EditorProcess::MainMenuBarFile()
	{
		if (!ImGui::BeginMenu("File")) return;
		if (ImGui::MenuItem("Save Scene", "CTRL+S"))       SaveActiveScene();
		if (ImGui::MenuItem("Save Scene As"))              SaveActiveScene(true);
		if (ImGui::MenuItem("Open Scene", "CTRL+O")) OpenScene();
		ImGui::EndMenu();
	}

	void EditorProcess::MainMenuBarEdit()
	{
		if (!ImGui::BeginMenu("Edit")) return;

		if (ImGui::MenuItem("Undo", "CTRL+Z", false, m_ActionHandler->CanUndo())) m_ActionHandler->Undo();
		if (ImGui::MenuItem("Redo", "CTRL+Y", false, m_ActionHandler->CanRedo())) m_ActionHandler->Redo();

		ImGui::EndMenu();
	}

	void EditorProcess::MainMenuBarHelp()
	{
		if (!ImGui::BeginMenu("Help")) return;

		// TODO: this

		if (ImGui::MenuItem("Scene Hierachy")) {

		}
		if (ImGui::MenuItem("Scene View")) {

		}
		if (ImGui::MenuItem("Inspector")) {

		}

		ImGui::EndMenu();
	}

	void EditorProcess::SaveActiveScene(bool dialog)
	{
		if (m_ActiveScene->GetFileName().empty() || dialog) {
			m_ActiveScene->GetFileName() = Core::System::SaveFileDialog("Enigma Scene (*.scene)\0*.scene\0", m_WindowID);
		}

		// Make sure filepath has the proper extension
		std::filesystem::path scenePath = m_ActiveScene->GetFileName();
		if (!scenePath.has_extension() || scenePath.extension() != SceneSerializer::FileExtension) {
			m_ActiveScene->GetFileName().append(SceneSerializer::FileExtension);
		}

		SceneSerializer serializer(m_ActiveScene);
		serializer.Serialize(m_ActiveScene->GetFileName());
	}
	void EditorProcess::OpenScene()
	{
		std::string scenePath = Core::System::OpenFileDialog("Enigma Scene (*.scene)\0*.scene\0", m_WindowID);
		if (!scenePath.empty()) {
			m_ActiveScene = Scene::Create();
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(scenePath);

			Event::SceneChange e(m_ActiveScene);
			Core::Application::EventCallback(e);
		}
	}
}