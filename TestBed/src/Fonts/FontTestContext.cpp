#include "Fonts/FontTestContext.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Window.h>
#include <Enigma/Core/Event/WindowEvent.h>
#include <Enigma/Engine/InputCodes.h>

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

FontTestContext::FontTestContext(Core::ID windowID) : TestContext(windowID)
{
	LOG_MESSAGE("Started Font Test Context", 5);

	Core::Window* window = Core::Application::GetWindow(m_WindowID);
	m_Surface.scale.x = window->GetWidth();
	m_Surface.scale.y = window->GetHeight();

	Renderer::Render2DConfig renderConfig;
	renderConfig.surface = m_Surface;
	renderConfig.renderAPI = window->GetAPI();

	Core::Application::UseRenderAPI(window->GetAPI());

	m_RenderContext = new Renderer::Render2D(renderConfig);
	m_MainShader = m_RenderContext->GetMainShader();

	Renderer::ViewBox viewBox = Renderer::ViewBox::SurfaceViewBox(m_Surface);
	m_Camera = new Renderer::OrthographicCamera(viewBox, 4);

	m_CurveResolution = 25;

	Engine::InputConfig inputConfig;
	inputConfig.surface = &m_Surface;
	inputConfig.window = window;
	m_InputContext = Engine::Input::Create(inputConfig);

	m_ShowPoints = false;
	m_Font = new Font("assets/fonts/JetBrainsMono-Medium.ttf");
	m_Text.font = m_Font;
	m_Text.SetText("Text:)");
	m_TextVAO = m_Text.GenerateVAO();
	m_TextPosition = { -2.5,-0.75 };
}
glm::vec2 FontTestContext::lerp(const glm::vec2& p0, const glm::vec2& p1, float t)
{
	return ((1 - t) * p0) + (p1 * t);
}

//void FontTestContext::GenerateBezierCurve()
//{
//	if (m_CurveVAO != nullptr) delete m_CurveVAO;
//	m_CurveVAO = Renderer::VertexArray::Create();
//	m_CurveVAO->Bind();
//	
//	struct vertex {
//		glm::vec3 position;
//		glm::vec2 texCoord;
//	};
//	std::vector<vertex> vertices;
//	std::vector<uint32_t> indices;
//
//	vertices.push_back({ { m_Point1, -1 }, { 0,0 } });
//	indices.push_back(vertices.size() - 1);
//
//	if (m_CurveResolution > 0) {
//		float interval = 1.0f / ((float)m_CurveResolution + 1.0f);
//		for (int i = 0; i < m_CurveResolution; ++i) {
//			float t = interval * (float)(i + 1);
//			glm::vec2 L0 = lerp(m_Point1, m_ControlPoint1, t);
//			glm::vec2 L1 = lerp(m_ControlPoint1, m_ControlPoint2, t);
//			glm::vec2 L2 = lerp(m_ControlPoint2, m_Point2, t);
//			glm::vec2 Q0 = lerp(L0, L1, t);
//			glm::vec2 Q1 = lerp(L1, L2, t);
//			glm::vec2 C0 = lerp(Q0, Q1, t);
//			vertices.push_back({ { C0, -1 }, { 0,0 } });
//			indices.push_back(vertices.size() - 1);
//		}
//	}
//
//	vertices.push_back({ { m_Point2, -1 }, { 0,0 } });
//	indices.push_back(vertices.size() - 1);
//
//	Renderer::VertexBuffer* vbo = Renderer::VertexBuffer::Create({ Renderer::DataType::Float3, Renderer::DataType::Float2 }, Renderer::Usage::StaticDraw);
//	vbo->SetData(&vertices[0], vertices.size() * sizeof(vertex));
//	vbo->InitAttribs();
//	m_CurveVAO->AttachBuffer(vbo);
//
//	Renderer::IndexBuffer* ebo = Renderer::IndexBuffer::Create(Renderer::DataType::UnsignedInt, Renderer::Usage::StaticDraw);
//	ebo->SetData(&indices[0], indices.size() * sizeof(uint32_t));
//	m_CurveVAO->AttachBuffer(ebo);
//
//	m_CurveVAO->Unbind();
//}
//
//bool FontTestContext::IsMouseHovering(const glm::vec2& point)
//{
//	glm::vec2 mousePos = Engine::Input::GetMousePosition();
//	glm::vec2 mouseWorldPos = m_Camera->ScreenToWorld(mousePos, m_Surface);
//	float dist = glm::distance(point, mouseWorldPos);
//	return dist <= 0.05;
//}

void Text::SetText(const std::string& str)
{
	text = str;
	points.clear();
	indices.clear();

	glm::mat4 transform;
	transform = glm::scale(glm::mat4(1.0f), { 0.0015, 0.0015, 1 });

	float curser = 0;

	uint32_t indexOffset = 0;

	for (char& c : text) {
		uint16_t utf16 = static_cast<char16_t>(c);
		GlyphData& glyph = font->m_GlyphData[font->m_CharMapTable.subTables[0].Search(utf16)];
		if (glyph.isCompound) glyph = font->m_GlyphData[0];

		for (int i = 0; i < glyph.offsetCount; ++i) {
			glm::vec2 point = {
				(float)glyph.offsetsX[i],
				(float)glyph.offsetsY[i]
			};
			point = transform * glm::vec4(point, 0, 1);
			point.x += curser;
			points.push_back(point);
		}
		curser += 1;

		uint16_t contourStartIndex = indexOffset;
		for (uint16_t& contourEndIndex : glyph.endPointsOfContours) {

			for (int i = contourStartIndex; i < contourEndIndex + indexOffset + 1; ++i) {
				indices.push_back(i);
				if (i + 1 > contourEndIndex + indexOffset) indices.push_back(contourStartIndex);
				else indices.push_back(i + 1);
			}

			contourStartIndex = contourEndIndex + indexOffset + 1;
		}
		indexOffset = contourStartIndex;
	}
}

Renderer::VertexArray* Text::GenerateVAO()
{
	glm::mat4 transform;
	transform = glm::scale(glm::mat4(1.0f), { 0.0015, 0.0015, 1 });

	struct vertex {
		glm::vec3 position;
		glm::vec2 texCoord;
	};
	std::vector<vertex> vertices;

	for (glm::vec2& point : points) {
		vertices.push_back({ { point, -1 }, { 0,0 } });
	}

	Renderer::VertexArray* vao;
	vao = Renderer::VertexArray::Create();
	vao->Bind();

	Renderer::VertexBuffer* vbo = Renderer::VertexBuffer::Create({ Renderer::DataType::Float3, Renderer::DataType::Float2 }, Renderer::Usage::StaticDraw);
	vbo->SetData(&vertices[0], vertices.size() * sizeof(vertex));
	vbo->InitAttribs();
	vao->AttachBuffer(vbo);

	Renderer::IndexBuffer* ebo = Renderer::IndexBuffer::Create(Renderer::DataType::UnsignedInt, Renderer::Usage::StaticDraw);
	ebo->SetData(&indices[0], indices.size() * sizeof(uint32_t));
	vao->AttachBuffer(ebo);

	vao->Unbind();
	return vao;
}

void FontTestContext::OnResize(int width, int height)
{
	m_RenderContext->Resize(width, height);
	m_Surface.Resize(width, height);
}
void FontTestContext::OnEvent(Core::Event& e)
{
	Core::EventHandler handler(e);
	handler.Dispatch<Core::WindowResize>([&](Core::WindowResize& e) { OnResize(e.GetWidth(), e.GetHeight()); return false; });
}

void FontTestContext::Update(Engine::DeltaTime deltaTime)
{
	Engine::Input::MakeCurrent(m_InputContext);
}

void FontTestContext::Render()
{

	m_RenderContext->StartFrame(m_Camera);

	if (m_ShowPoints) {
		for (int i = 0; i < m_Text.points.size(); ++i) {
			glm::vec2 point = m_Text.points[i];
			point = glm::vec4(point, 0, 1);
			m_RenderContext->DrawQuad(point + m_TextPosition, { 0.015, 0.015 }, 0, 0, { 1,1,1,1 });
		}
	}

	Renderer::RenderAPI::SetDrawMode(Renderer::DrawMode::Lines);

	m_MainShader->Bind();
	m_MainShader->SetUniform("ViewProjection", (void*)&m_Camera->GetViewProjection());

	
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, { m_TextPosition, 0 });
	m_MainShader->SetUniform("Model", (void*)&transform);
	glm::vec4 tint(1.0f);
	m_MainShader->SetUniform("Tint", (void*)&tint);

	m_TextVAO->Bind();
	Renderer::RenderAPI::DrawIndexed(m_TextVAO->GetIndexBuffer()->GetIndexCount(), Renderer::DataType::UnsignedInt, NULL);
	m_TextVAO->Unbind();
	m_MainShader->Unbind();

	Renderer::RenderAPI::SetDrawMode(Renderer::DrawMode::Triangles);

	m_RenderContext->EndFrame();
}

void FontTestContext::ImGui()
{
	ImGui::Begin("Debug Menu");
	ImGui::Text("Font Test");
	ImGui::TextWrapped("Bezier Curve Demo");
	ImGui::InputInt("Curve Resolution", &m_CurveResolution);
	ImGui::DragFloat2("Text Position", &m_TextPosition.x);
	if (ImGui::InputText("Text", &m_Text.text) && !m_Text.text.empty()) {
		m_Text.SetText(m_Text.text);
		m_TextVAO = m_Text.GenerateVAO();
	}
	if (ImGui::Button("Update Text VAO")) {
		m_TextVAO = m_Text.GenerateVAO();
	}
	ImGui::Checkbox("Show Points", &m_ShowPoints);
	ImGui::End();
}