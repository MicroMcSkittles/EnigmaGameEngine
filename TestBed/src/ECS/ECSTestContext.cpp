#include "ECS/ECSTestContext.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Window.h>
#include <Enigma/Core/Event/WindowEvent.h>
#include <Enigma/Engine/InputCodes.h>
#include <EnigmaSerialization/Image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <random>

constexpr double PI = 3.14159265359;

struct Physics {
	glm::vec2 direction;
	float speed;
};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(0, 1000);

void ECSTestContext::NewEntity()
{
	Engine::ECS::EntityID entityID = m_ECS->CreateEntity();

	// Create a random transform component
	Engine::ECS::Transform& transform = m_ECS->CreateComponent<Engine::ECS::Transform>(entityID);
	transform.position = { (float)distrib(gen) / 1000.0f, (float)distrib(gen) / 1000.0f, -1.0f - (float)distrib(gen) / 1000.0f };
	transform.scale = { (float)distrib(gen) / 4000.0f, (float)distrib(gen) / 4000.0f, 1 };
	float rotation = (float)distrib(gen) / 1000.0f;
	transform.rotation = glm::rotate(glm::quat(), { 0, 0, rotation });

	// Create a random render component
	glm::vec3 tint = { (float)distrib(gen) / 1000.0f, (float)distrib(gen) / 1000.0f, 1 };
	switch (rand() % 4) {
	case 0: {
		Engine::ECS::ColoredQuad& quad = m_ECS->CreateComponent<Engine::ECS::ColoredQuad>(entityID);
		quad.tint = tint;
		break;
	}
	case 1: {
		Engine::ECS::TexturedQuad& quad = m_ECS->CreateComponent<Engine::ECS::TexturedQuad>(entityID);
		quad.tint = tint;
		quad.texture = m_Texture;
		break;
	}
	case 2: {
		Engine::ECS::ColoredCircle& circle = m_ECS->CreateComponent<Engine::ECS::ColoredCircle>(entityID);
		circle.tint = tint;
		break;
	}
	case 3: {
		Engine::ECS::TexturedCircle& circle = m_ECS->CreateComponent<Engine::ECS::TexturedCircle>(entityID);
		circle.tint = tint;
		circle.texture = m_Texture;
		break;
	}
	}

	// Create a random physics component
	Physics& physics = m_ECS->CreateComponent<Physics>(entityID);
	physics.direction = {
		cos(rotation - PI),
		sin(rotation - PI)
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

	if (m_RenderECS != nullptr) delete m_RenderECS;
	Engine::ECS::RenderSystem2DConfig renderConfig;
	renderConfig.surface = m_Surface;
	renderConfig.renderAPI = Core::Application::GetWindow(m_WindowID)->GetAPI();
	m_RenderECS = new Engine::ECS::RenderSystem2D(renderConfig, m_ECS);

	if (m_Texture != nullptr) delete m_Texture;
	Serialization::ImageConfig textureConfig;
	m_Texture = Serialization::ImageLoader::Load("assets/test.png", textureConfig);

	for (size_t i = 0; i < m_EntityCount; ++i) {
		NewEntity();
	}
}

void ECSTestContext::UpdateTest(Engine::DeltaTime deltaTime)
{
	if (!m_Running) return;

	Engine::ECS::View<Engine::ECS::Transform, Physics> view(m_ECS);

	view.ForEach([&](Engine::ECS::EntityID entityID, Engine::ECS::Transform& transform, Physics& physics) {
		transform.position += glm::vec3(physics.direction * physics.speed * (float)deltaTime, 0);
		float dist = glm::length(transform.position);
		if (dist >= 4) {
			transform.position.x = 0;
			transform.position.y = 1;
		}
	});
}

void ECSTestContext::Render()
{
	m_RenderECS->StartFrame(m_Camera);

	m_RenderECS->EndFrame();

	/*return;
	if (!m_Running) return;
	m_RenderContext->StartFrame(m_Camera);

	Engine::ECS::View<Transform, Color> view(m_ECS);
	view.ForEach([&](Transform& transform, Color& color) {
		m_RenderContext->DrawQuad(transform.position, transform.scale, transform.rotation, 0, { color.color, 1 });
	});

	m_RenderContext->EndFrame();*/
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
	//Renderer::Render2DConfig renderConfig;
	//renderConfig.surface = m_Surface;
	//renderConfig.renderAPI = window->GetAPI();
	//m_RenderContext = new Renderer::Render2D(renderConfig);

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
	m_RenderECS = nullptr;
	m_Texture = nullptr;

	StartTest();
}

void ECSTestContext::OnResize(int width, int height)
{
	//m_RenderContext->Resize(width, height);
	m_RenderECS->Resize(width, height);
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

