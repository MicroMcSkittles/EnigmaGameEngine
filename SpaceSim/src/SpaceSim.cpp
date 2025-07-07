#include "SpaceSim.h"

#include <Enigma/Core/Process/Application.h>
#include <Enigma/Engine/InputCodes.h>
#include <imgui.h>

#include "SpaceRenderer.h"

void SpaceSim::StartUp() {

	Core::WindowConfig windowConfig;
	windowConfig.width = 1600;
	windowConfig.height = 1600;
	windowConfig.renderAPI = Renderer::API::OpenGL;
	windowConfig.title = "Kerbal Space Pro... I mean space sim";
	windowConfig.resizable = false;

	Core::ImGuiConfig imguiConfig;
	imguiConfig.docking = false;

	m_WindowID = Core::Application::CreateWindow(windowConfig, imguiConfig);
	Core::Application::BindSubProcToWindow(this, m_WindowID);
	Core::Application::GetWindow(m_WindowID)->AddEventCallback([&](Core::Event& e) { return OnEvent(e); });

	m_Surface.scale = { 1600,1600 };

	SpaceRenderer::Init(&m_Surface);

	Engine::InputConfig inputConfig;
	inputConfig.surface = &m_Surface;
	inputConfig.window = Core::Application::GetWindow(m_WindowID);
	m_InputContext = Engine::Input::Create(inputConfig);

	// Init player
	Renderer::Frustum frustum = Renderer::Frustum::SurfaceFrustum(m_Surface);
	frustum.far = 100000;
	m_Camera = new Renderer::PerspectiveCamera(frustum, { 50,25,50 }, glm::normalize(glm::vec3(-50,-25,-50)));
	m_Player = new Player(m_Camera);

	// The Real G const is 6.6743e-11
	m_Simulation = new Simulation(0.1);
	m_Simulation->AddPlanet({ { 0,0,0 }, { 0,0,0 }, 50, 4,   { 0.25,1,0.12 } });
	m_SelectedPlanet = m_Simulation->AddPlanet({ { 17,0,0 }, { 0,0,3.5 }, 3, 1, { 0.75,0.75,0.75 } });
	//m_SelectedPlanet = m_Simulation->AddPlanet({ { 0,15,0 }, { 3,0,0 }, 1, 0.25, { 0.0,0.75,0.75 } });

	LOG_MESSAGE("%s", 5, Core::Application::GetArgument(0).c_str());

	LOG_MESSAGE("Starting Space Sim", 5);
}

void SpaceSim::Update(Engine::DeltaTime deltaTime) {
	m_MS = deltaTime * 1000.0f;
	m_FPS = 1 / deltaTime;
	Engine::Input::MakeCurrent(m_InputContext);
	m_Player->Update(deltaTime);
	m_Simulation->Update(deltaTime);
}

void SpaceSim::Render() {
	SpaceRenderer::StartFrame(m_Camera);

	for (Planet& planet : m_Simulation->GetPlanets()) {
		SpaceRenderer::Submit({ planet.position, planet.radius, planet.color });
	}

	SpaceRenderer::Submit(m_Simulation->GetGrid());

	SpaceRenderer::EndFrame();
}

void SpaceSim::ImGui()
{
	ImGui::Begin("Debug Menu", 0, ImGuiWindowFlags_NoTitleBar);
	ImGui::SeparatorText("Stats");
	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Text("MS: %.2f", m_MS);

	ImGui::Separator();
	ImGui::TextWrapped("Hold Right-Click to move and look around");
	ImGui::TextWrapped("W, A, S, D, E, Q to move");

	ImGui::End();

	m_Simulation->ImGui();
}

void SpaceSim::OnResize(Core::WindowResize& e)
{
	SpaceRenderer::Resize(e.GetWidth(), e.GetHeight());
}
void SpaceSim::OnKeyboard(Core::Keyboard& e) {
	if (!e.IsShortcut(Engine::KeyCode::KeyR, Engine::KeyCode::CtrlMask)) return;
	SpaceRenderer::ReloadShaders();
}
void SpaceSim::OnClose() {
	Core::Application::Close();
}
bool SpaceSim::OnEvent(Core::Event& e) {
	Core::EventHandler handler(e);
	handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& e) { OnClose(); return false; });
	handler.Dispatch<Core::WindowResize>([&](Core::WindowResize& e) { OnResize(e); return false; });
	handler.Dispatch<Core::Keyboard>([&](Core::Keyboard& e) { OnKeyboard(e); return false; });
	return e.Handled();
}