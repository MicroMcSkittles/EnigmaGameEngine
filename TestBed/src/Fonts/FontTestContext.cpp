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
	INIT_PROFILER(25);

	// Setup surface
	ref<Core::Window> window = Core::Application::GetWindow(m_WindowID);
	m_Surface.scale.x = window->GetWidth();
	m_Surface.scale.y = window->GetHeight();

	// Setup renderer
	Renderer::Render2DConfig renderConfig;
	renderConfig.surface = m_Surface;
	renderConfig.renderAPI = window->GetAPI();
	m_RenderContext = new Renderer::Render2D(renderConfig);

	// Create camera
	Renderer::ViewBox viewBox = Renderer::ViewBox::SurfaceViewBox(m_Surface);
	m_Camera = Renderer::OrthographicCamera::Create(viewBox, 3, {1.5,1.5,0});

	// Create input context
	Engine::InputConfig inputConfig;
	inputConfig.surface = ref<Engine::Surface>(&m_Surface);
	inputConfig.window = window;
	m_InputContext = Engine::Input::Create(inputConfig);

	// Init debug values
	m_ShowCurvePoints   = false;
	m_ShowImpliedPoints = false;
	m_ShowControlPoints = false;
	m_ShowGlyph         = true;
	m_ShowBounds        = false;
	
	m_CameraSpeed = 2.5;
	m_DebugElementsSize = 0.25;

	// Init font and text
	m_Font      = Font::Load("assets/fonts/JetBrainsMono-Medium.ttf");
	m_TextValue = Core::System::ReadFileStr("assets/stdfonttest.txt");
	m_Text      = m_Font->GenerateText(m_TextValue);
	m_TextColor = { 1,1,1 };
}

void FontTestContext::OnScroll(Core::MouseScroll& e)
{
	// Handle zooming in and out
	if (!m_Controlling) return;
	float zoom = m_Camera->GetZoom();
	zoom -= e.GetY() * (1.0f / 16.0f);
	m_Camera->SetZoom(zoom);
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
	handler.Dispatch<Core::MouseScroll>([&](Core::MouseScroll& e) { OnScroll(e); return false; });
}

void FontTestContext::Update(Engine::DeltaTime deltaTime)
{
	m_FPS = 1.0f / deltaTime;
	PROFILE();
	Engine::Input::MakeCurrent(m_InputContext);

	// Handle camera input
	if (!Engine::Input::IsMouseButtonPressed(Engine::KeyCode::MouseButtonRight)) {
		m_Controlling = false;
		return;
	}
	m_Controlling = true;

	glm::vec2 moveDir = { 0,0 };
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyW)) moveDir += glm::vec2( 0, 1);
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyS)) moveDir += glm::vec2( 0,-1);
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyA)) moveDir += glm::vec2( -1, 0);
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyD)) moveDir += glm::vec2(1, 0);
	if (moveDir.x != 0 || moveDir.y != 0) {
		moveDir = glm::normalize(moveDir);
		glm::vec2 newCameraPos = m_Camera->GetPosition();
		newCameraPos = newCameraPos + (moveDir * (float)deltaTime) * (m_CameraSpeed * m_Camera->GetZoom());
		m_Camera->SetPosition(glm::vec3(newCameraPos, 0));
	}
}

void FontTestContext::Render()
{
	START_PROFILE("Start Frame");
	m_RenderContext->StartFrame(m_Camera);
	END_PROFILE();

	// Render Text
	float textSize = 1;
	if (m_ShowGlyph) m_RenderContext->DrawText(m_Text, { 0,0 }, textSize, 0, -1, { m_TextColor, 1 });

	// Render debug info
	glm::mat4 transform = glm::scale(glm::mat4(1.0f), { textSize, textSize, 1 });
	RenderTextDebugElements(transform);

	START_PROFILE("End Frame");
	m_RenderContext->EndFrame();
	END_PROFILE();

	END_PROFILE_FRAME();
}
void FontTestContext::RenderBoundingBox(const Renderer::TextBoundingBox& boundingBox, const glm::vec2& position, float scale)
{
	ADDITIVE_PROFILE("Render Bounding Box");
	// Transform text bounding box
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, { position,0 });
	transform = glm::scale(transform, { scale, scale, 1 });

	glm::vec2 textSize     = glm::vec4(boundingBox.size * scale, 0, 1);
	glm::vec2 textMinBound = transform * glm::vec4(boundingBox.min, 0, 1);
	glm::vec2 textMaxBound = transform * glm::vec4(boundingBox.max, 0, 1);
	glm::vec2 textMidpoint = transform * glm::vec4(boundingBox.position, 0, 1);

	// Draw bounding box and its points
	m_RenderContext->DrawLineQuad(textMidpoint, textSize, 0, 0.25 * m_DebugElementsSize, -2, { 0.5,0.5,0.5, 1 });
	m_RenderContext->DrawCircle(textMidpoint, 0.06 * m_DebugElementsSize, 0, { 0,1,1,1 });
	m_RenderContext->DrawCircle(textMinBound, 0.06 * m_DebugElementsSize, 0, { 1,0,1,1 });
	m_RenderContext->DrawCircle(textMaxBound, 0.06 * m_DebugElementsSize, 0, { 1,0,1,1 });
}
void FontTestContext::RenderTextPoint(const Renderer::TextPoint& point, const glm::mat4& transform)
{
	ADDITIVE_PROFILE("Render Text Point");
	glm::vec4 tint = { 1,1,1,1 };
	bool control = point.type & Renderer::TextPointType::TextPointType_Control;
	bool implied = point.type & Renderer::TextPointType::TextPointType_Implied;
	bool curve = point.type & Renderer::TextPointType::TextPointType_OnCurve;
	bool start = point.type & Renderer::TextPointType::TextPointType_StartOfContour;
	if (m_ShowControlPoints && control)             tint = { 1,0,0,1 };
	else if (m_ShowImpliedPoints && implied)             tint = { 0,1,0,1 };
	else if (m_ShowControlPoints && start)               tint = { 0,0,1,1 };
	else if (m_ShowCurvePoints && start)               tint = { 0,0,1,1 };
	else if (m_ShowCurvePoints && curve && !implied)   tint = { 1,1,1,1 };
	else return;

	glm::vec2 position = transform * glm::vec4(point.position, 0, 1);
	m_RenderContext->DrawCircle(position, 0.03 * m_DebugElementsSize, 0, tint);
}
void FontTestContext::RenderTextDebugElements(const glm::mat4& transform)
{
	PROFILE("Render Text Debug Elements");
	bool showPoints = m_ShowCurvePoints || m_ShowImpliedPoints || m_ShowControlPoints;
	if (!showPoints && !m_ShowBounds) return;

	if (m_ShowBounds) RenderBoundingBox(m_Text->GetBoundingBox(), { 0,0 }, 1);

	float scale = 0.0005;

	// Get spaceing data
	float verticalSpacing        = (float)m_Font->GetVerticalSpacing()        * scale;
	float spaceHorizontalAdvance = (float)m_Font->GetSpaceHorizontalAdvance() * scale;

	glm::vec2 glyphOffset = { 0,0 };
	std::vector<Renderer::Glyph> glyphs;
	std::map<uint16_t, uint32_t> glyphLookupTable;

	for (char c : m_TextValue) {
		// Process spaces
		if (c == ' ') {
			glyphOffset.x += spaceHorizontalAdvance;
			continue;
		}
		// process new lines
		if (c == '\n') {
			glyphOffset.x = 0;
			glyphOffset.y -= verticalSpacing;
			continue;
		}

		// If character glyph hasn't been used yet then get the glyph and map the unicode to the glyph index
		if (!glyphLookupTable.count(c)) {
			glyphLookupTable.insert({ c, glyphs.size() });
			glyphs.push_back(m_Font->GetGlyph(c));
		}

		Renderer::Glyph& glyph = glyphs[glyphLookupTable[c]];

		glm::mat4 glyphTransform = glm::mat4(1.0f);
		glyphTransform = glm::translate(glyphTransform, { glyphOffset, 0 });
		glyphTransform = glm::scale(glyphTransform, { scale, scale, 1 });
		glyphTransform = transform * glyphTransform;

		if (m_ShowBounds) RenderBoundingBox(glyph.boundingBox, glyphOffset, scale);

		if (showPoints) {
			// Loop through each point
			for (Renderer::TextPoint& point : glyph.points) {
				RenderTextPoint(point, glyphTransform);
			}
		}

		glyphOffset.x += (float)glyph.horizontalAdvance * scale;
	}
}

void FontTestContext::ImGui()
{
	SHOW_PROFILER_IMGUI();

	ImGui::Begin("Debug Menu");
	ImGui::Text("Font Test");
	ImGui::TextWrapped("Text Rendering From Fonts Demo");

	ImGui::Text("FPS: %f", m_FPS);

	if (ImGui::TreeNode("Text")) {
		if (ImGui::Button("Open Font...")) {
			std::string rslt = Core::System::OpenFileDialog("Font Files (*.ttf)\0", m_WindowID);
			if (!rslt.empty()) {
				delete m_Font; 
				m_Font = Font::Load(rslt);
				delete m_Text;
				m_Text = m_Font->GenerateText(m_TextValue);
			}
		}
		if (ImGui::Button("Open Text File...")) {
			std::string rslt = Core::System::OpenFileDialog("Text Files (*.txt)\0", m_WindowID);
			if (!rslt.empty()) {
				delete m_Text;
				m_TextValue = Core::System::ReadFileStr(rslt);
				m_Text = m_Font->GenerateText(m_TextValue);
			}
		}
		if (ImGui::InputTextMultiline("Text", &m_TextValue)) {
			if (!m_TextValue.empty()) {
				delete m_Text;
				m_Text = m_Font->GenerateText(m_TextValue);
			}
		}
		ImGui::ColorEdit3("Text Color", &m_TextColor.x);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Debug")) {
		ImGui::Checkbox("Show Glyph", &m_ShowGlyph);
		ImGui::Checkbox("Show Bounds", &m_ShowBounds);
		ImGui::SeparatorText("Points");
		ImGui::TextWrapped("Showing all the points at once can be very laggy (at least for right now). Note, The debug elements function is the bottle neck not the Renderer.");
		ImGui::Checkbox("Show Curve Points", &m_ShowCurvePoints);
		ImGui::Checkbox("Show Implied Points", &m_ShowImpliedPoints);
		ImGui::Checkbox("Show Control Points", &m_ShowControlPoints);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Other")) {
		ImGui::DragFloat("Debug Elements Size", &m_DebugElementsSize, 0.01);
		ImGui::DragFloat("Camera Speed", &m_CameraSpeed, 0.1f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Help")) {
		ImGui::TextWrapped("Hold the right mouse button and press WASD to move or Scroll to zoom in and out");
		ImGui::TextWrapped("The color of each point has a meaning.");
		ImGui::BulletText("White: Point on the curve");
		ImGui::BulletText("Green: Implied point (not stored in ttf file)");
		ImGui::BulletText("Red: Control point");
		ImGui::BulletText("Blue: Start of the contour");
		ImGui::BulletText("Magenta: Min and Max points of a bounding box");
		ImGui::BulletText("Cyan: Center point of a glyph");
		ImGui::TreePop();
	}

	ImGui::End();
}