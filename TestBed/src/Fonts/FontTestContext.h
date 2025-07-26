#pragma once
#include "TestBed.h"
#include <Enigma/Renderer/Render2D.h>
#include <Enigma/Engine/Input.h>

#include <EnigmaSerialization/Font.h>
using namespace Enigma::Serialization;

struct Curve {
	std::vector<uint32_t> points;
	int resolution;
};
struct Text {
	std::vector<glm::vec2> points;
	//std::vector<Curve> curves;
	std::vector<uint32_t> indices;
	std::string text;
	Font* font;

	void SetText(const std::string& str);
	Renderer::VertexArray* GenerateVAO();
};

class FontTestContext : public TestContext {
public:
	FontTestContext(Core::ID windowID);

	void OnResize(int width, int height);
	virtual void OnEvent(Core::Event& e) override;
	virtual void Update(Engine::DeltaTime deltaTime) override;
	virtual void Render() override;
	virtual void ImGui() override;

private:
	glm::vec2 lerp(const glm::vec2& p0, const glm::vec2& p1, float t);
	void GenerateGlyph(int index);

private:
	Renderer::OrthographicCamera* m_Camera;
	Renderer::Shader* m_MainShader;

	Renderer::Render2D* m_RenderContext;
	Engine::Surface m_Surface;

	Engine::Input* m_InputContext;

	Renderer::VertexArray* m_TextVAO;
	int m_CurveResolution;
	bool m_ShowPoints;
	Text m_Text;
	glm::vec2 m_TextPosition;
	Font* m_Font;

};