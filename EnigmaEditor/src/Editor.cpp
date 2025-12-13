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
		m_EditorScene = Scene::Create();
		SceneSerializer serializer(m_EditorScene);
		serializer.Deserialize("assets/Scene.scene");
		m_ActiveScene = m_EditorScene;

		// Create action handler
		m_EditorActionHandler = CreateUnique<ActionHandler>();

		// Create Panels
		m_InspectorPanel = CreateUnique<InspectorPanel>();

		m_SceneViewPanel = CreateUnique<SceneViewPanel>(m_WindowID);
		m_SceneViewPanel->SetContext(m_ActiveScene);

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
			Event::NewInspectorContext contextEvent(EntityInspectorContext::Create(e.GetEntity(), m_ActiveScene));
			Core::Application::EventCallback(contextEvent);
			return false; 
		});

		handler.Dispatch<Event::StartRuntime>([&](Event::StartRuntime& e) { StartRuntime(); return false; });
		handler.Dispatch<Event::PauseRuntime>([&](Event::PauseRuntime& e) { PauseRuntime(); return false; });

		if (m_EditorState == EditorState_Editing) m_EditorActionHandler->OnEvent(e);
		else if (m_EditorState == EditorState_Running) m_RuntimeActionHandler->OnEvent(e);

		m_SceneViewPanel->OnEvent(e);
		m_SceneHierachyPanel->OnEvent(e);
		m_InspectorPanel->OnEvent(e);

		return false;
	}
	void EditorProcess::StartRuntime() {
		m_EditorState = EditorState_Running;
		m_RuntimeScene = CreateRef<Scene>(*m_EditorScene.get());
		m_ActiveScene = m_RuntimeScene;

		Event::SceneChange e(m_ActiveScene);
		Core::Application::EventCallback(e);

		m_ActiveScene->StartRuntime();

		m_RuntimeActionHandler = CreateUnique<ActionHandler>();

		EditorGui::TintStyle(0.5f);
		LOG_MESSAGE("Started Scene Runtime", 5);
	}
	void EditorProcess::PauseRuntime() {
		m_EditorState = EditorState_Editing;
		m_ActiveScene = m_EditorScene;
		m_RuntimeScene = nullptr;

		Event::SceneChange e(m_ActiveScene);
		Core::Application::EventCallback(e);

		m_ActiveScene->EndRuntime();

		m_RuntimeActionHandler = nullptr;
		EditorGui::RemoveStyleTint();

		LOG_MESSAGE("Paused Scene Runtime", 5);
	}

	void EditorProcess::Update(Engine::DeltaTime deltaTime)
	{
		m_SceneViewPanel->Update(deltaTime);
		m_ActiveScene->Update(deltaTime);
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
		ActionHandler* activeActionHandler;
		if (m_EditorState == EditorState_Editing) activeActionHandler = m_EditorActionHandler.get();
		else if (m_EditorState == EditorState_Running) activeActionHandler = m_RuntimeActionHandler.get();
		if (ImGui::Shortcut(ImGuiKey_Z | ImGuiMod_Ctrl, shortcutFlags) && activeActionHandler->CanUndo()) activeActionHandler->Undo();
		if (ImGui::Shortcut(ImGuiKey_Y | ImGuiMod_Ctrl, shortcutFlags) && activeActionHandler->CanRedo()) activeActionHandler->Redo();

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
		if (ImGui::MenuItem("Save Scene", "CTRL+S")) SaveActiveScene();
		if (ImGui::MenuItem("Save Scene As"))        SaveActiveScene(true);
		if (ImGui::MenuItem("Open Scene", "CTRL+O", nullptr, m_EditorState == EditorState_Editing)) OpenScene();
		ImGui::EndMenu();
	}

	void EditorProcess::MainMenuBarEdit()
	{
		if (!ImGui::BeginMenu("Edit")) return;

		ActionHandler* activeActionHandler;
		if (m_EditorState == EditorState_Editing) activeActionHandler = m_EditorActionHandler.get();
		else if (m_EditorState == EditorState_Running) activeActionHandler = m_RuntimeActionHandler.get();
		if (ImGui::MenuItem("Undo", "CTRL+Z", false, activeActionHandler->CanUndo())) activeActionHandler->Undo();
		if (ImGui::MenuItem("Redo", "CTRL+Y", false, activeActionHandler->CanRedo())) activeActionHandler->Redo();

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
		if (m_EditorScene->GetFileName().empty() || dialog) {
			m_EditorScene->GetFileName() = Core::System::SaveFileDialog("Enigma Scene (*.scene)\0*.scene\0", m_WindowID);
		}

		// Make sure filepath has the proper extension
		std::filesystem::path scenePath = m_EditorScene->GetFileName();
		if (!scenePath.has_extension() || scenePath.extension() != SceneSerializer::FileExtension) {
			m_EditorScene->GetFileName().append(SceneSerializer::FileExtension);
		}

		SceneSerializer serializer(m_EditorScene);
		serializer.Serialize(m_EditorScene->GetFileName());
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