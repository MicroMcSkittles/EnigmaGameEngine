#pragma once
#include "TestBed.h"
#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Renderer/Renderer2D/Render2D.h>
#include <Enigma/Engine/Input.h>

#include <EnigmaSerialization/Fonts/FontTTF.h>
#include <EnigmaSerialization/Fonts/Font.h>
using namespace Enigma::Serialization;

#define CURRENT_TEST_CONTEXT FontTestContext
#define CURRENT_TEST_CONTEXT_NAME "Font Test"

class FontTestContext : public TestContext {
public:
	FontTestContext(Core::ID windowID);

	void OnScroll(Core::MouseScroll& e);
	void OnResize(int width, int height);
	virtual void OnEvent(Core::Event& e) override;
	virtual void Update(Engine::DeltaTime deltaTime) override;
	virtual void Render() override;
	virtual void ImGui() override;

private:
	void RenderBoundingBox(const Renderer::TextBoundingBox& boundingBox, const glm::vec2& position, float scale);
	void RenderTextPoint(const Renderer::TextPoint& point, const glm::mat4& transform);
	void RenderTextDebugElements(const glm::mat4& transform);

private:
	// Render
	ref<Renderer::OrthographicCamera> m_Camera;
	Renderer::Render2D* m_RenderContext;
	Engine::Surface m_Surface;

	// Input
	ref<Engine::Input> m_InputContext;
	float m_CameraSpeed;
	bool m_Controlling;

	// Debug values
	float m_FPS;
	float m_DebugElementsSize;
	bool m_ShowCurvePoints;
	bool m_ShowImpliedPoints;
	bool m_ShowControlPoints;
	bool m_ShowBounds;
	bool m_ShowGlyph;
	
	// Text values
	Font* m_Font;
	std::string m_TextValue;
	Renderer::Text* m_Text;
	glm::vec3 m_TextColor;
};