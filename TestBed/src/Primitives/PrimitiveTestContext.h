#pragma once
#include "TestBed.h"

#include <Enigma/Renderer/Renderer2D/Render2D.h>
#include <Enigma/Core/Event/WindowEvent.h>

#define CURRENT_TEST_CONTEXT PrimitiveTestContext
#define CURRENT_TEST_CONTEXT_NAME "Primitive Test"

using namespace Enigma;

class PrimitiveTestContext : public TestContext {
public:
	PrimitiveTestContext(Core::ID windowID);

	void OnResize(Core::WindowResize& e);
	virtual void OnEvent(Core::Event& e) override;
	virtual void Update(Engine::DeltaTime deltaTime) { }
	virtual void Render() override;
	virtual void ImGui() override;

private:
	float m_Radius;
	glm::vec2 m_Position;
	glm::vec4 m_Tint;
	Renderer::Texture* m_Texture;

	Engine::Surface m_Surface;
	Renderer::Render2D* m_RenderContext;
	Renderer::OrthographicCamera* m_Camera;
};