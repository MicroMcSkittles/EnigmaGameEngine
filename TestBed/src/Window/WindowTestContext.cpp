#include "Window/WindowTestContext.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Window.h>
#include <Enigma/Core/Event/WindowEvent.h>
#include <Enigma/Engine/InputCodes.h>
#include <imgui.h>

WindowTestContext::WindowTestContext(Core::ID windowID) : TestContext(windowID)
{
	LOG_MESSAGE("Started Window Test Context", 5);

	Core::Application::UseRenderAPI(Renderer::API::OpenGL);
	Renderer::RenderAPI::SetClearColor({ 0,0,0,1 });
	Renderer::RenderAPI::SetClearMask(Renderer::ClearMask::ColorBufferBit);
	Renderer::RenderAPI::SetViewport(800, 600);

	m_Surface.scale = { 800, 600 };

	Engine::InputConfig inputConfig;
	inputConfig.window = Core::Application::GetWindow(m_WindowID);
	inputConfig.surface = ref<Engine::Surface>(&m_Surface);
	m_InputContext = Engine::Input::Create(inputConfig);
}

void WindowTestContext::OnResize(int width, int height)
{
	m_Surface.Resize(width, height);
}

void WindowTestContext::OnEvent(Core::Event& e)
{
	if (e.GetType() == Core::EventType::MouseMoved) return;
	std::cout << e.ToString() << std::endl;

	Core::EventHandler handler(e);
	handler.Dispatch<Core::WindowResize>([&](Core::WindowResize& e) { OnResize(e.GetWidth(), e.GetHeight()); return false; });
}

void WindowTestContext::Update(Engine::DeltaTime deltaTime)
{
	Engine::Input::MakeCurrent(m_InputContext);
	glm::vec2 mousePos = Engine::Input::GetMousePosition();
	mousePos /= m_Surface.scale;
	Renderer::RenderAPI::SetClearColor({ mousePos,0,1 });

	if (Engine::Input::IsMouseButtonPressed(Engine::KeyCode::MouseButtonMiddle)) {
		Core::Application::Close();
	}
}

void WindowTestContext::Render()
{
	Renderer::RenderAPI::Clear();
}

void WindowTestContext::ImGui()
{
	ImGui::Begin("Test Window");
	ImGui::Text("wow");
	ImGui::End();
}