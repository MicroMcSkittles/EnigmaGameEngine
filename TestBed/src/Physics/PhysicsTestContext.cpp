#include "Physics/PhysicsTestContext.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Window.h>
#include <Enigma/Core/Event/WindowEvent.h>
#include <Enigma/Engine/InputCodes.h>

#include <glm/gtx/euler_angles.hpp>

#include <imgui.h>

PhysicsTestContext::PhysicsTestContext(Core::ID windowID) : TestContext(windowID)
{
	LOG_MESSAGE("Started Entity Component System Test Context", 5);

	// Setup surface
	Core::Window* window = Core::Application::GetWindow(m_WindowID);
	m_Surface.scale.x = window->GetWidth();
	m_Surface.scale.y = window->GetHeight();

	m_ECS = new Engine::ECS::ECS();
	m_PhysicsEngine = new Engine::Physics::PhysicsEngine2D(m_ECS);
	m_FPS = 0;
	m_Running = true;
	m_InputQuaternion = false;
	m_InputDegrees = true;

	// Setup renderer
	Engine::ECS::RenderSystem2DConfig renderConfig;
	renderConfig.surface = m_Surface;
	renderConfig.renderAPI = window->GetAPI();
	m_RenderContext = new Engine::ECS::RenderSystem2D(renderConfig, m_ECS);

	// Create camera
	Renderer::ViewBox viewBox = Renderer::ViewBox::SurfaceViewBox(m_Surface);
	m_Camera = new Renderer::OrthographicCamera(viewBox, 8);

	// Create input context
	Engine::InputConfig inputConfig;
	inputConfig.surface = &m_Surface;
	inputConfig.window = window;
	m_InputContext = Engine::Input::Create(inputConfig);

	// Setup scene
	using namespace Engine::ECS;
	using namespace Engine::Physics;
	{
		m_Cuboid = m_ECS->CreateEntity();

		// Create transform component
		Transform& transform = m_ECS->CreateComponent<Transform>(m_Cuboid);
		transform.position = { 0.0, 0.0, -1.0 };
		transform.rotation = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		transform.scale = { 1.0, 1.0, 1.0 };

		// Create render component
		ColoredQuad& quad = m_ECS->CreateComponent<ColoredQuad>(m_Cuboid);
		quad.tint = { 1.0, 1.0, 1.0 };

		// Create ridgid body component
		RidgidBody2D& ridgidBody = m_ECS->CreateComponent<RidgidBody2D>(m_Cuboid);
		ridgidBody.linearVelocity = { 0.0, 0.0 };
		ridgidBody.angularVelocity = 0.0;
		ridgidBody.rotationMatrix = glm::mat3_cast(transform.rotation);

		//Collider2D& collider = m_ECS->CreateComponent<Collider2D>(m_Cuboid);
		//collider = BoxCollider2D;
	}
	{
		m_Cuboid1 = m_ECS->CreateEntity();

		// Create transform component
		Transform& transform = m_ECS->CreateComponent<Transform>(m_Cuboid1);
		transform.position = { 5.0, 0.0, -1.0 };
		transform.rotation = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		transform.scale = { 1.0, 1.0, 1.0 };

		// Create render component
		ColoredQuad& quad = m_ECS->CreateComponent<ColoredQuad>(m_Cuboid1);
		quad.tint = { 1.0, 1.0, 1.0 };

		// Create ridgid body component
		RidgidBody2D& ridgidBody = m_ECS->CreateComponent<RidgidBody2D>(m_Cuboid1);
		ridgidBody.linearVelocity = { 0.0, 0.0 };
		ridgidBody.angularVelocity = 0.0;
		ridgidBody.rotationMatrix = glm::mat3_cast(transform.rotation);

		//Collider2D& collider = m_ECS->CreateComponent<Collider2D>(m_Cuboid1);
		//collider = BoxCollider2D;
	}
}

void PhysicsTestContext::Update(Engine::DeltaTime deltaTime)
{
	m_FPS = 1.0f / deltaTime;
	Engine::Input::MakeCurrent(m_InputContext);

	m_PhysicsEngine->Update(deltaTime);
}

void PhysicsTestContext::Render()
{
	m_RenderContext->StartFrame(m_Camera);

	m_RenderContext->EndFrame();
}

void PhysicsTestContext::ImGui()
{
	ImGui::Begin("Debug Menu");
	ImGui::Text("Physics Test");
	ImGui::TextWrapped("Physics Demo");

	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Text("Entity count: %d", (int)m_ECS->GetEntityCount());

	ImGui::SeparatorText("Miscellaneous");
	ImGui::Checkbox("Rotation inputs as quaternions", &m_InputQuaternion);
	ImGui::Checkbox("Rotation inputs as degrees", &m_InputDegrees);

	ImGui::SeparatorText("Cuboid");

	using namespace Engine::ECS;
	using namespace Engine::Physics;
	Transform&    transform  = m_ECS->GetComponent<Transform>(m_Cuboid);
	ColoredQuad&  quad       = m_ECS->GetComponent<ColoredQuad>(m_Cuboid);
	RidgidBody2D& ridgidBody = m_ECS->GetComponent<RidgidBody2D>(m_Cuboid);

	if (ImGui::TreeNode("Transform")) {
		ImGui::InputFloat3("Position", &transform.position.x);

		if (m_InputQuaternion) {
			ImGui::InputFloat4("Rotation", &transform.rotation.x);
		}
		else {
			glm::vec3 rotation = glm::eulerAngles(transform.rotation);
			if (m_InputDegrees) {
				rotation.x = glm::degrees(rotation.x);
				rotation.y = glm::degrees(rotation.y);
				rotation.z = glm::degrees(rotation.z);
			}
			if (ImGui::InputFloat3("Rotation", &rotation.x)) {
				if (m_InputDegrees) {
					rotation.x = glm::radians(rotation.x);
					rotation.y = glm::radians(rotation.y);
					rotation.z = glm::radians(rotation.z);
				}
				transform.rotation = glm::rotate(glm::quat(), rotation);
			}
		}

		ImGui::InputFloat3("Scale", &transform.scale.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Render Quad")) {
		ImGui::ColorEdit3("Color", &quad.tint.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Ridgid Body 2D")) {
		ImGui::InputFloat2("Linear Velocity", &ridgidBody.linearVelocity.x);
		ImGui::InputFloat("Angular Velocity", &ridgidBody.angularVelocity);
		ImGui::TreePop();
	}

	ImGui::End();
}

void PhysicsTestContext::OnResize(int width, int height)
{
	m_RenderContext->Resize(width, height);
	m_Surface.Resize(width, height);
}
void PhysicsTestContext::OnEvent(Core::Event& e)
{
	Core::EventHandler handler(e);
	handler.Dispatch<Core::WindowResize>([&](Core::WindowResize& e) { OnResize(e.GetWidth(), e.GetHeight()); return false; });
}