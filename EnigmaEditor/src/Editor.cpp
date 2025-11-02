#include "Editor.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Event/WindowEvent.h>

#include <imgui.h>

#include "EditorImGui.h"
#include <Enigma/Engine/ECS/Components.h>

namespace Enigma::Editor {
	void EditorProcess::StartUp()
	{
		// Create Window
		Core::WindowConfig windowConfig;
		windowConfig.title     = "Enigma Editor";
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

		m_SceneHierachyPanel = CreateUnique<SceneHierachyPanel>();
		m_SceneHierachyPanel->SetContext(m_Scene);
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

		return false;
	}

	void EditorProcess::Update(Engine::DeltaTime deltaTime)
	{
	}
	void EditorProcess::Render()
	{
		Renderer::RenderAPI::Clear();
	}

	void EditorProcess::ImGui()
	{
		ImGui::ShowDemoWindow();
		m_SceneHierachyPanel->ShowGui();
	}
}