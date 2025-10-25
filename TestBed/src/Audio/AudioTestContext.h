#pragma once
#include "TestBed.h"
#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Renderer/Renderer2D/Render2D.h>
#include <Enigma/Engine/Input.h>

#define CURRENT_TEST_CONTEXT AudioTestContext
#define CURRENT_TEST_CONTEXT_NAME "Audio Test"

class AudioTestContext : public TestContext {
public:
	AudioTestContext(Core::ID windowID);
	~AudioTestContext();

	void OnResize(int width, int height);
	void OnClose();
	virtual void OnEvent(Core::Event& e) override;
	virtual void Update(Engine::DeltaTime deltaTime) override;
	virtual void Render() override;
	virtual void ImGui() override;

private:
	// Render
	ref<Renderer::OrthographicCamera> m_Camera;
	Renderer::Render2D* m_RenderContext;
	Engine::Surface m_Surface;

	// Input
	ref<Engine::Input> m_InputContext;
};