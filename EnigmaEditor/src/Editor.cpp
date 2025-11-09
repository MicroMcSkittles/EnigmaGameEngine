#include "Editor.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Event/WindowEvent.h>

#include <imgui.h>
#include <ImGuizmo.h>

#include "EditorImGui.h"
#include <Enigma/Engine/ECS/Components.h>

namespace Enigma::Editor {
	void EditorProcess::StartUp()
	{
		// Create Window
		Core::WindowConfig windowConfig;
		windowConfig.title     = "Enigma Editor";
		windowConfig.width     = 1024;
		windowConfig.height    = 720;
		windowConfig.renderAPI = Renderer::API::OpenGL;

		Core::ImGuiConfig imguiConfig;
		imguiConfig.renderAPI = windowConfig.renderAPI;

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

		// Setup scene
		m_Scene = Scene::Create();
		m_Entity = m_Scene->CreateEntity("Cube");
		m_Scene->CreateEntity(m_Entity, "Child 1");
		m_Scene->CreateEntity(m_Entity, "Child 2");
		m_Scene->CreateEntity("Box even");
		m_Scene->CreateEntity("Circle");

		m_Entity.CreateComponent<Engine::ECS::ColoredQuad>(glm::vec3(1.0f, 0.0f, 0.0f));

		m_InspectorPanel = CreateUnique<InspectorPanel>();

		m_SceneViewPanel = CreateUnique<SceneViewPanel>(m_WindowID);
		m_SceneViewPanel->SetContext(m_Scene);

		m_SceneHierachyPanel = CreateUnique<SceneHierachyPanel>();
		m_SceneHierachyPanel->SetContext(m_Scene);
		m_SceneHierachyPanel->SetSelectionCallback([&](Entity selected) {
			m_SceneViewPanel->SetSelected(selected);
			m_InspectorPanel->SetContext(EntityInspectorContext::Create(selected));
		});

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

		m_SceneViewPanel->OnEvent(e);

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

		m_InspectorPanel->ShowGui();
		m_SceneHierachyPanel->ShowGui();
		m_SceneViewPanel->ShowGui();
	}
}