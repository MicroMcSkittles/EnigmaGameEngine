#pragma once
#include "TestBed.h"
#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Renderer/Renderer2D/Render2D.h>
#include <Enigma/Engine/Input.h>

#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/RenderSystem2D.h>

#include <Enigma/Engine/Physics/PhysicsEngine2D.h>

#define CURRENT_TEST_CONTEXT PhysicsTestContext
#define CURRENT_TEST_CONTEXT_NAME "Physics Test"

class PhysicsTestContext : public TestContext {
public:
	PhysicsTestContext(Core::ID windowID);

	void OnResize(int width, int height);
	virtual void OnEvent(Core::Event& e) override;
	virtual void Update(Engine::DeltaTime deltaTime) override;
	virtual void Render() override;
	virtual void ImGui() override;

private:
	// Render
	Renderer::OrthographicCamera* m_Camera;
	Engine::ECS::RenderSystem2D* m_RenderContext;
	Engine::Surface m_Surface;

	// Input
	Engine::Input* m_InputContext;
	float m_CameraSpeed;
	bool m_Controlling;

	// Test variables
	Engine::ECS::ECS* m_ECS;
	Engine::ECS::EntityID m_Cuboid;
	Engine::ECS::EntityID m_Cuboid1;
	Engine::Physics::PhysicsEngine2D* m_PhysicsEngine;
	bool m_Running;
	float m_FPS;

	bool m_InputQuaternion;
	bool m_InputDegrees;
};