#pragma once
#include "TestBed.h"
#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Renderer/Renderer2D/Render2D.h>
#include <Enigma/Engine/Input.h>

#define CURRENT_TEST_CONTEXT WindowTestContext
#define CURRENT_TEST_CONTEXT_NAME "Window Test"

class WindowTestContext : public TestContext {
public:
	WindowTestContext(Core::ID windowID);

	void OnResize(int width, int height);
	virtual void OnEvent(Core::Event& e) override;
	virtual void Update(Engine::DeltaTime deltaTime) override;
	virtual void Render() override;
	virtual void ImGui() override;

private:
	// Input
	Engine::Surface m_Surface;
	ref<Engine::Input> m_InputContext;
};