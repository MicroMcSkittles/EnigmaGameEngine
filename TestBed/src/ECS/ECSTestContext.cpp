#include "ECS/ECSTestContext.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Window.h>
#include <Enigma/Core/Event/WindowEvent.h>
#include <Enigma/Engine/InputCodes.h>

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <random>
#include <iostream>

constexpr double PI = 3.14159265359;

struct Transform {
	glm::vec2 position;
	float rotation;
	glm::vec2 scale;
};
struct Physics {
	glm::vec2 direction;
	float speed;
};
struct Color {
	glm::vec3 color;
};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(0, 1000);

void ECSTestContext::NewEntity()
{
	Engine::ECS::EntityID entityID = m_ECS->CreateEntity();

	Transform& transform = m_ECS->CreateComponent<Transform>(entityID);
	transform.position = { (float)distrib(gen) / 1000.0f, (float)distrib(gen) / 1000.0f };
	transform.scale = { (float)distrib(gen) / 4000.0f, (float)distrib(gen) / 4000.0f };
	transform.rotation = (float)distrib(gen) / 1000.0f;

	Color& color = m_ECS->CreateComponent<Color>(entityID);
	color.color = { (float)distrib(gen) / 1000.0f, (float)distrib(gen) / 1000.0f, 1 };

	Physics& physics = m_ECS->CreateComponent<Physics>(entityID);
	physics.direction = {
		cos(transform.rotation - PI),
		sin(transform.rotation - PI)
	};
	if (rand() % 2) physics.direction.x *= -1.0f;
	if (rand() % 2) physics.direction.y *= -1.0f;
	physics.direction = glm::normalize(physics.direction);
	physics.speed = (float)distrib(gen) / 1000.0f;
}

void ECSTestContext::StartTest()
{
	if (m_ECS != nullptr) delete m_ECS;
	m_ECS = new Engine::ECS::ECS();

	for (size_t i = 0; i < m_EntityCount; ++i) {
		NewEntity();
	}
}

void ECSTestContext::UpdateTest(Engine::DeltaTime deltaTime)
{
	if (!m_Running) return;

	auto view = m_ECS->GetView<Transform, Physics>();
	view.ForEach([&](Engine::ECS::EntityID entityID, Transform& transform, Physics& physics) {
		transform.position += physics.direction * physics.speed * (float)deltaTime;
		float dist = glm::length(transform.position);
		if (dist >= 4) {
			Transform t = transform;
			t.position = { 0,0 };
			Physics p = physics;
			m_ECS->RemoveEntity(entityID);
			Engine::ECS::EntityID id = m_ECS->CreateEntity();
			m_ECS->CreateComponent<Transform>(id) = t;
			m_ECS->CreateComponent<Physics>(id) = p;
		}
	});
}

void ECSTestContext::Render()
{
	if (!m_Running) return;
	m_RenderContext->StartFrame(m_Camera);

	auto view = m_ECS->GetView<Transform, Color>();
	view.ForEach([&](Transform& transform, Color& color) {
		m_RenderContext->DrawQuad(transform.position, transform.scale, transform.rotation, 0, { color.color, 1 });
	});

	m_RenderContext->EndFrame();
}

void ECSTestContext::ImGui()
{
	ImGui::Begin("Debug Menu");
	ImGui::Text("Entity Component System Test");
	ImGui::TextWrapped("Entity Component System Demo");

	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Text("Entity count: %d", (int)m_ECS->GetEntityCount());
	if (ImGui::Button("Restart")) {
		StartTest();
	}
	ImGui::InputInt("Starting Entity Count", &m_EntityCount);
	ImGui::Checkbox("Running", &m_Running);

	ImGui::End();
}

ECSTestContext::ECSTestContext(Core::ID windowID) : TestContext(windowID)
{
	LOG_MESSAGE("Started Entity Component System Test Context", 5);

	// Setup surface
	Core::Window* window = Core::Application::GetWindow(m_WindowID);
	m_Surface.scale.x = window->GetWidth();
	m_Surface.scale.y = window->GetHeight();

	// Setup renderer
	Renderer::Render2DConfig renderConfig;
	renderConfig.surface = m_Surface;
	renderConfig.renderAPI = window->GetAPI();
	m_RenderContext = new Renderer::Render2D(renderConfig);

	// Create camera
	Renderer::ViewBox viewBox = Renderer::ViewBox::SurfaceViewBox(m_Surface);
	m_Camera = new Renderer::OrthographicCamera(viewBox, 3);

	// Create input context
	Engine::InputConfig inputConfig;
	inputConfig.surface = &m_Surface;
	inputConfig.window = window;
	m_InputContext = Engine::Input::Create(inputConfig);

	m_FPS = 0;
	m_Running = true;
	m_EntityCount = 1000;
	m_ECS = nullptr;

	StartTest();
}

void ECSTestContext::OnResize(int width, int height)
{
	m_RenderContext->Resize(width, height);
	m_Surface.Resize(width, height);
}
void ECSTestContext::OnEvent(Core::Event& e)
{
	Core::EventHandler handler(e);
	handler.Dispatch<Core::WindowResize>([&](Core::WindowResize& e) { OnResize(e.GetWidth(), e.GetHeight()); return false; });
}

void ECSTestContext::Update(Engine::DeltaTime deltaTime)
{
	m_FPS = 1.0f / deltaTime;
	Engine::Input::MakeCurrent(m_InputContext);
	UpdateTest(deltaTime);
}

