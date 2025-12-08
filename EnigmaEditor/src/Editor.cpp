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
		m_ActionHandler = CreateUnique<ActionHandler>();

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

		m_ActionHandler->OnEvent(e);

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

		EditorStyle& style = EditorGui::GetStyle();
		style.windowBackground = glm::vec4(0.05f, 0.0525f, 0.055f, 1.0f);
		style.header = glm::vec4(0.2f / 2.0f, 0.205f / 2.0f, 0.21f / 2.0f, 1.0f);
		style.headerHovered = glm::vec4(0.3f / 2.0f, 0.305f / 2.0f, 0.31f / 2.0f, 1.0f);
		style.headerActive = glm::vec4(0.15f / 2.0f, 0.1505f / 2.0f, 0.151f / 2.0f, 1.0f);
		style.button = glm::vec4(0.2f / 2.0f, 0.205f / 2.0f, 0.21f / 2.0f, 1.0f);
		style.buttonHovered = glm::vec4(0.3f / 2.0f, 0.305f / 2.0f, 0.31f / 2.0f, 1.0f);
		style.buttonActive = glm::vec4(0.15f / 2.0f, 0.1505f / 2.0f, 0.151f / 2.0f, 1.0f);

		// Frame Background Colors
		style.frameBackground = glm::vec4(0.2f / 2.0f, 0.205f / 2.0f, 0.21f / 2.0f, 1.0f);
		style.frameBackgroundHovered = glm::vec4(0.3f / 2.0f, 0.305f / 2.0f, 0.31f / 2.0f, 1.0f);
		style.frameBackgroundActive = glm::vec4(0.15f / 2.0f, 0.1505f / 2.0f, 0.151f / 2.0f, 1.0f);

		// Tab Colors
		style.tab = glm::vec4(0.15f / 2.0f, 0.1505f / 2.0f, 0.151f / 2.0f, 1.0f);
		style.tabHovered = glm::vec4(0.38f / 2.0f, 0.3805f / 2.0f, 0.381f / 2.0f, 1.0f);
		style.tabActive = glm::vec4(0.28f / 2.0f, 0.2805f / 2.0f, 0.281f / 2.0f, 1.0f);
		style.tabUnfocused = glm::vec4(0.15f / 2.0f, 0.1505f / 2.0f, 0.151f / 2.0f, 1.0f);
		style.tabUnfocusedActive = glm::vec4(0.2f / 2.0f, 0.205f / 2.0f, 0.21f / 2.0f, 1.0f);

		// Title Background Colors
		style.titleBackground = glm::vec4(0.15f / 2.0f, 0.1505f / 2.0f, 0.151f / 2.0f, 1.0f);
		style.titleBackgroundActive = glm::vec4(0.15f / 2.0f, 0.1505f / 2.0f, 0.151f / 2.0f, 1.0f);
		style.titleBackgroundCollapsed = glm::vec4(0.95f / 2.0f, 0.1505f / 2.0f, 0.951f / 2.0f, 1.0f);

		// Float Input Colors
		style.colorX = glm::vec4(0.8f / 2.0f, 0.1f / 2.0f, 0.15f / 2.0f, 1.0f);
		style.colorY = glm::vec4(0.2f / 2.0f, 0.7f / 2.0f, 0.2f / 2.0f, 1.0f);
		style.colorZ = glm::vec4(0.1f / 2.0f, 0.25f / 2.0f, 0.8f / 2.0f, 1.0f);
		style.colorW = glm::vec4(0.8f / 2.0f, 0.15f / 2.0f, 0.8f / 2.0f, 1.0f);

		style.pressedColorX = glm::vec4(0.9f / 2.0f, 0.2f / 2.0f, 0.2f / 2.0f, 1.0f);
		style.pressedColorY = glm::vec4(0.3f / 2.0f, 0.8f / 2.0f, 0.3f / 2.0f, 1.0f);
		style.pressedColorZ = glm::vec4(0.2f / 2.0f, 0.35f / 2.0f, 0.9f / 2.0f, 1.0f);
		style.pressedColorW = glm::vec4(0.9f / 2.0f, 0.25f / 2.0f, 0.8f / 2.0f, 1.0f);
		
		EditorGui::SetStyle(EditorGui::GetStyle());
	}
	void EditorProcess::PauseRuntime() {
		m_EditorState = EditorState_Editing;
		m_ActiveScene = m_EditorScene;
		m_RuntimeScene = nullptr;

		Event::SceneChange e(m_ActiveScene);
		Core::Application::EventCallback(e);

		m_ActiveScene->EndRuntime();

		EditorStyle& style = EditorGui::GetStyle();

		style.windowBackground = glm::vec4(0.1f, 0.105f, 0.11f, 1.0f);

		// Header Colors
		style.header = glm::vec4(0.2f, 0.205f, 0.21f, 1.0f);
		style.headerHovered = glm::vec4(0.3f, 0.305f, 0.31f, 1.0f);
		style.headerActive = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Button Colors
		style.button = glm::vec4(0.2f, 0.205f, 0.21f, 1.0f);
		style.buttonHovered = glm::vec4(0.3f, 0.305f, 0.31f, 1.0f);
		style.buttonActive = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Frame Background Colors
		style.frameBackground = glm::vec4(0.2f, 0.205f, 0.21f, 1.0f);
		style.frameBackgroundHovered = glm::vec4(0.3f, 0.305f, 0.31f, 1.0f);
		style.frameBackgroundActive = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Tab Colors
		style.tab = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);
		style.tabHovered = glm::vec4(0.38f, 0.3805f, 0.381f, 1.0f);
		style.tabActive = glm::vec4(0.28f, 0.2805f, 0.281f, 1.0f);
		style.tabUnfocused = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);
		style.tabUnfocusedActive = glm::vec4(0.2f, 0.205f, 0.21f, 1.0f);

		// Title Background Colors
		style.titleBackground = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);
		style.titleBackgroundActive = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);
		style.titleBackgroundCollapsed = glm::vec4(0.95f, 0.1505f, 0.951f, 1.0f);

		// Float Input Colors
		style.colorX = glm::vec4(0.8f, 0.1f, 0.15f, 1.0f);
		style.colorY = glm::vec4(0.2f, 0.7f, 0.2f, 1.0f);
		style.colorZ = glm::vec4(0.1f, 0.25f, 0.8f, 1.0f);
		style.colorW = glm::vec4(0.8f, 0.15f, 0.8f, 1.0f);

		style.pressedColorX = glm::vec4(0.9f, 0.2f, 0.2f, 1.0f);
		style.pressedColorY = glm::vec4(0.3f, 0.8f, 0.3f, 1.0f);
		style.pressedColorZ = glm::vec4(0.2f, 0.35f, 0.9f, 1.0f);
		style.pressedColorW = glm::vec4(0.9f, 0.25f, 0.8f, 1.0f);

		EditorGui::SetStyle(EditorGui::GetStyle());
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
		if (ImGui::MenuItem("Save Scene", "CTRL+S")) SaveActiveScene();
		if (ImGui::MenuItem("Save Scene As"))        SaveActiveScene(true);
		if (ImGui::MenuItem("Open Scene", "CTRL+O", nullptr, m_EditorState == EditorState_Editing)) OpenScene();
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