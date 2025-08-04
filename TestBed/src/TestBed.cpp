#include "TestBed.h"

#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Event/WindowEvent.h>

//#include "Fonts/FontTestContext.h"
#include "Primitives/PrimitiveTestContext.h"


void TestBed::StartUp()
{
	Core::WindowConfig windowConfig;
	windowConfig.width = 800;
	windowConfig.height = 600;
	windowConfig.title = CURRENT_TEST_CONTEXT_NAME;
	windowConfig.renderAPI = Renderer::API::OpenGL;

	Core::ImGuiConfig imguiConfig;
	imguiConfig.docking = false;

	m_WindowID = Core::Application::CreateWindow(windowConfig, imguiConfig);
	Core::Application::BindSubProcToWindow(this, m_WindowID);
	Core::Application::GetWindow(m_WindowID)->AddEventCallback([&](Core::Event& e) { return OnEvent(e); });
  
  LOG_MESSAGE("Test, hello!!", 5);

	m_TestContext = new CURRENT_TEST_CONTEXT(m_WindowID);
}

void TestBed::OnClose()
{
	Core::Application::Close();
}
bool TestBed::OnEvent(Core::Event& e)
{
	Core::EventHandler handler(e);
	handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& e) { OnClose(); return false; });
	
	m_TestContext->OnEvent(e);

	return false;
}

void TestBed::Update(Engine::DeltaTime deltaTime)
{
	m_TestContext->Update(deltaTime);
}
void TestBed::Render()
{
	m_TestContext->Render();
}
void TestBed::ImGui()
{
	m_TestContext->ImGui();
}
