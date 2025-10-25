#pragma once
#include "TestBed.h"
#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Renderer/Renderer2D/Render2D.h>
#include <Enigma/Engine/Input.h>

#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/RenderSystem2D.h>

#define CURRENT_TEST_CONTEXT ECSTestContext
#define CURRENT_TEST_CONTEXT_NAME "Entity Component System Test"

class ECSTestContext : public TestContext {
public:
	ECSTestContext(Core::ID windowID);

	void NewEntity();
	void StartTest();
	void UpdateTest(Engine::DeltaTime deltaTime);

	void OnResize(int width, int height);
	virtual void OnEvent(Core::Event& e) override;
	virtual void Update(Engine::DeltaTime deltaTime) override;
	virtual void Render() override;
	virtual void ImGui() override;

private:
	// Render
	ref<Renderer::OrthographicCamera> m_Camera;
	Renderer::Render2D* m_RenderContext;
	Engine::Surface m_Surface;

	ref<Engine::ECS::RenderSystem2D> m_RenderECS;

	// Input
	ref<Engine::Input> m_InputContext;
	float m_CameraSpeed;
	bool m_Controlling;

	// Test variables
	ref<Engine::ECS::ECS> m_ECS;
	ref<Renderer::Texture> m_Texture;
	bool m_Running;
	int m_EntityCount;
	float m_FPS;
};